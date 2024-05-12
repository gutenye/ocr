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

#include "det_process.h"
// #include <onnxruntime_cxx_api.h>
#include <format>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "db_post_process.h"
#include "timer.h"

// resize image to a size multiple of 32 which is required by the network
cv::Mat DetResizeImg(const cv::Mat img, int max_size_len, std::vector<float> &ratio_hw) {
  int w = img.cols;
  int h = img.rows;
  float ratio = 1.f;
  int max_wh = w >= h ? w : h;
  if (max_wh > max_size_len) {
    if (h > w) {
      ratio = static_cast<float>(max_size_len) / static_cast<float>(h);
    } else {
      ratio = static_cast<float>(max_size_len) / static_cast<float>(w);
    }
  }

  int resize_h = static_cast<int>(float(h) * ratio);
  int resize_w = static_cast<int>(float(w) * ratio);
  if (resize_h % 32 == 0)
    resize_h = resize_h;
  else if (resize_h / 32 < 1 + 1e-5)
    resize_h = 32;
  else
    resize_h = (resize_h / 32 - 1) * 32;

  if (resize_w % 32 == 0)
    resize_w = resize_w;
  else if (resize_w / 32 < 1 + 1e-5)
    resize_w = 32;
  else
    resize_w = (resize_w / 32 - 1) * 32;
  cv::Mat resize_img;

  cv::resize(img, resize_img, cv::Size(resize_w, resize_h));

  ratio_hw.push_back(static_cast<float>(resize_h) / static_cast<float>(h));
  ratio_hw.push_back(static_cast<float>(resize_w) / static_cast<float>(w));
  return resize_img;
}

DetPredictor::DetPredictor(Options &options, const int cpuThreadNum, const std::string &cpuPowerMode)
    : m_options {options} {}

ImageRaw DetPredictor::Preprocess(const cv::Mat &srcimg, const int max_side_len) {
  cv::Mat img = DetResizeImg(srcimg, max_side_len, ratio_hw_);
  cv::Mat img_fp;
  img.convertTo(img_fp, CV_32FC3, 1.0 / 255.f);

  std::vector<float> data0(img_fp.rows * img_fp.cols * 3);
  std::vector<float> mean = {0.485f, 0.456f, 0.406f};
  std::vector<float> scale = {1 / 0.229f, 1 / 0.224f, 1 / 0.225f};
  const float *dimg = reinterpret_cast<const float *>(img_fp.data);
  NHWC3ToNC3HW(dimg, data0.data(), img_fp.rows * img_fp.cols, mean, scale);

  ImageRaw image_raw {.data = data0, .width = img_fp.cols, .height = img_fp.rows, .channels = 3};

  return image_raw;
}

std::vector<std::vector<std::vector<int>>> DetPredictor::Postprocess(ModelOutput &model_output, const cv::Mat &srcimg,
                                                                     Options &options) {
  auto height = model_output.shape[2];
  auto width = model_output.shape[3];
  cv::Mat pred_map = cv::Mat(height, width, CV_32F, model_output.data.data());
  cv::Mat cbuf_map;
  pred_map.convertTo(cbuf_map, CV_8UC1, 255.0f);

  const double threshold = options.detection_threshold * 255;
  const double max_value = 255;
  cv::Mat bit_map;
  cv::threshold(cbuf_map, bit_map, threshold, max_value, cv::THRESH_BINARY);
  if (options.detection_use_dilate == 1) {
    cv::Mat dilation_map;
    cv::Mat dila_ele = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
    cv::dilate(bit_map, dilation_map, dila_ele);
    bit_map = dilation_map;
  }
  auto boxes = BoxesFromBitmap(pred_map, bit_map, options);

  std::vector<std::vector<std::vector<int>>> filter_boxes = FilterTagDetRes(boxes, ratio_hw_[0], ratio_hw_[1], srcimg);

  return filter_boxes;
}

std::vector<std::vector<std::vector<int>>> DetPredictor::Predict(cv::Mat &img) {
  cv::Mat srcimg;
  img.copyTo(srcimg);

  Timer tic;
  tic.start();
  auto image = Preprocess(img, m_options.image_max_size);
  tic.end();
  auto preprocessTime = tic.get_average_ms();
  std::cout << "det predictor preprocess costs " << preprocessTime << std::endl;

  // Run predictor
  std::vector<int64_t> input_shape = {1, image.channels, image.height, image.width};

  Onnx onnx {m_options.detection_model_path};
  tic.start();
  auto model_output = onnx.run(image.data, input_shape);
  tic.end();
  auto predictTime = tic.get_average_ms();
  std::cout << "det predictor predict costs " << predictTime << std::endl;

  // Process Output
  tic.start();
  auto filter_boxes = Postprocess(model_output, srcimg, m_options);
  tic.end();
  auto postprocessTime = tic.get_average_ms();
  std::cout << "det predictor postprocess costs " << postprocessTime << std::endl;

  return filter_boxes;
}
