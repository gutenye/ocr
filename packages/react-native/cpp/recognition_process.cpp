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

#include "recognition_process.h"
#include <format>
#include <iostream>
#include "shared.h"
#include "timer.h"
#include "utils.h"

cv::Mat resize_image(cv::Mat image, int image_height);
template <class ForwardIterator>
inline size_t argmax(ForwardIterator first, ForwardIterator last);

RecognitionPredictor::RecognitionPredictor(Options &options, const int cpu_thread_num,
                                           const std::string &cpu_power_mode)
    : m_options {options}, m_onnx {Onnx {options.recognition_model_path}} {}

RecognitionResult RecognitionPredictor::predict(const cv::Mat &rgba_image, std::vector<std::string> charactor_dict) {
  ModelPerformance performance {};
  Timer timer;
  timer.start();
  auto image = preprocess(rgba_image);
  timer.end();
  performance.preprocess_time = timer.get_average_ms();

  // Run predictor
  std::vector<int64_t> input_shape = {1, image.channels, image.height, image.width};
  timer.start();
  auto model_output = m_onnx.run(image.data, input_shape);
  timer.end();
  performance.predict_time = timer.get_average_ms();

  timer.start();
  auto res = postprocess(model_output, rgba_image, charactor_dict);
  timer.end();
  auto postprocessTime = timer.get_average_ms();
  performance.postprocess_time = timer.get_average_ms();

  performance.total_time = performance.preprocess_time + performance.predict_time + performance.postprocess_time;

  return RecognitionResult {.data = res, .performance = performance};
}

ImageRaw RecognitionPredictor::preprocess(const cv::Mat &source_image) {
  std::vector<float> mean = {0.5f, 0.5f, 0.5f};
  std::vector<float> scale = {1 / 0.5f, 1 / 0.5f, 1 / 0.5f};
  // cv::Mat resized_image = resize_image(source_image, 32);
  cv::Mat resized_image = resize_image(source_image, 48);
  resized_image.convertTo(resized_image, CV_32FC3, 1 / 255.f);

  const float *destination_image = reinterpret_cast<const float *>(resized_image.data);
  std::vector<float> data(resized_image.rows * resized_image.cols * 3);
  NHWC3ToNC3HW(destination_image, data.data(), resized_image.rows * resized_image.cols, mean, scale);

  ImageRaw image_raw {.data = data, .width = resized_image.cols, .height = resized_image.rows, .channels = 3};

  return image_raw;
}

std::pair<std::string, float> RecognitionPredictor::postprocess(ModelOutput &model_output, const cv::Mat &rgba_image,
                                                                std::vector<std::string> charactor_dict) {
  auto predict_batch = model_output.data;
  auto predict_shape = model_output.shape;

  // ctc decode
  std::string text;
  int argmax_idx;
  int last_index = 0;
  float score = 0.f;
  int count = 0;
  float max_value = 0.0f;

  for (int n = 0; n < predict_shape[1]; n++) {
    argmax_idx = int(argmax(&predict_batch[n * predict_shape[2]], &predict_batch[(n + 1) * predict_shape[2]]));
    max_value =
        float(*std::max_element(&predict_batch[n * predict_shape[2]], &predict_batch[(n + 1) * predict_shape[2]]));
    if (argmax_idx > 0 && (!(n > 0 && argmax_idx == last_index))) {
      score += max_value;
      count += 1;
      text += charactor_dict[argmax_idx];
    }
    last_index = argmax_idx;
  }
  score /= count;
  return std::make_pair(text, score);
}

cv::Mat resize_image(cv::Mat image, int image_height) {
  float wh_ratio = static_cast<float>(image.cols) / static_cast<float>(image.rows);
  auto image_width = int(ceilf(image_height * wh_ratio));
  cv::Mat resize_image;
  cv::resize(image, resize_image, cv::Size(image_width, image_height), 0.f, 0.f, cv::INTER_LINEAR);
  return resize_image;
}

template <class ForwardIterator>
inline size_t argmax(ForwardIterator first, ForwardIterator last) {
  return std::distance(first, std::max_element(first, last));
}