// Copyright (c) 2021 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "rec_process.h"
#include <format>
#include <iostream>
#include "shared.h"
#include "timer.h"
#include "utils.h"

const std::vector<int> rec_image_shape {3, 48, 320};

cv::Mat CrnnResizeImg(cv::Mat img, float wh_ratio) {
  int imgC, imgH, imgW;
  imgC = rec_image_shape[0];
  imgW = rec_image_shape[2];
  imgH = rec_image_shape[1];

  // imgW = int(32 * wh_ratio);
  imgW = int(48 * wh_ratio);

  float ratio = static_cast<float>(img.cols) / static_cast<float>(img.rows);
  int resize_w, resize_h;
  if (ceilf(imgH * ratio) > imgW)
    resize_w = imgW;
  else
    resize_w = static_cast<int>(ceilf(imgH * ratio));
  cv::Mat resize_img;
  cv::resize(img, resize_img, cv::Size(resize_w, imgH), 0.f, 0.f, cv::INTER_LINEAR);

  return resize_img;
}

template <class ForwardIterator>
inline size_t Argmax(ForwardIterator first, ForwardIterator last) {
  return std::distance(first, std::max_element(first, last));
}

RecPredictor::RecPredictor(Options &options, const int cpuThreadNum, const std::string &cpuPowerMode)
    : m_options {options} {}

ImageRaw RecPredictor::Preprocess(const cv::Mat &srcimg) {
  float wh_ratio = static_cast<float>(srcimg.cols) / static_cast<float>(srcimg.rows);
  std::vector<float> mean = {0.5f, 0.5f, 0.5f};
  std::vector<float> scale = {1 / 0.5f, 1 / 0.5f, 1 / 0.5f};
  cv::Mat resize_img = CrnnResizeImg(srcimg, wh_ratio);
  resize_img.convertTo(resize_img, CV_32FC3, 1 / 255.f);

  const float *dimg = reinterpret_cast<const float *>(resize_img.data);

  std::vector<float> data0(resize_img.rows * resize_img.cols * 3);
  NHWC3ToNC3HW(dimg, data0.data(), resize_img.rows * resize_img.cols, mean, scale);

  ImageRaw image_raw {.data = data0, .width = resize_img.cols, .height = resize_img.rows, .channels = 3};

  return image_raw;
}

std::pair<std::string, float> RecPredictor::Postprocess(ModelOutput &model_output, const cv::Mat &rgbaImage,
                                                        std::vector<std::string> charactor_dict) {
  auto predict_batch = model_output.data;
  auto predict_shape = model_output.shape;

  // ctc decode
  std::string str_res;
  int argmax_idx;
  int last_index = 0;
  float score = 0.f;
  int count = 0;
  float max_value = 0.0f;

  for (int n = 0; n < predict_shape[1]; n++) {
    argmax_idx = int(Argmax(&predict_batch[n * predict_shape[2]], &predict_batch[(n + 1) * predict_shape[2]]));
    max_value =
        float(*std::max_element(&predict_batch[n * predict_shape[2]], &predict_batch[(n + 1) * predict_shape[2]]));
    if (argmax_idx > 0 && (!(n > 0 && argmax_idx == last_index))) {
      score += max_value;
      count += 1;
      str_res += charactor_dict[argmax_idx];
    }
    last_index = argmax_idx;
  }
  score /= count;
  return std::make_pair(str_res, score);
}

std::pair<std::string, float> RecPredictor::Predict(const cv::Mat &rgbaImage, std::vector<std::string> charactor_dict) {
  Timer tic;
  tic.start();
  auto image = Preprocess(rgbaImage);
  tic.end();
  auto preprocessTime = tic.get_average_ms();
  // std::cout << "rec predictor preprocess costs " << preprocessTime << std::endl;

  // Run predictor
  std::vector<int64_t> input_shape = {1, image.channels, image.height, image.width};
  Onnx onnx {m_options.recognition_model_path};
  tic.start();
  auto model_output = onnx.run(image.data, input_shape);
  tic.end();
  auto predictTime = tic.get_average_ms();
  // std::cout << "rec predictor predict costs " << predictTime << std::endl;

  tic.start();
  auto res = Postprocess(model_output, rgbaImage, charactor_dict);
  tic.end();
  auto postprocessTime = tic.get_average_ms();
  // std::cout << "rec predictor postprocess costs " << postprocessTime << std::endl;

  return res;
}
