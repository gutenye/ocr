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

const std::vector<int> recognition_image_shape {3, 48, 320};
cv::Mat crnn_resize_image(cv::Mat image, float wh_ratio);
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
  float wh_ratio = static_cast<float>(source_image.cols) / static_cast<float>(source_image.rows);
  std::vector<float> mean = {0.5f, 0.5f, 0.5f};
  std::vector<float> scale = {1 / 0.5f, 1 / 0.5f, 1 / 0.5f};
  cv::Mat resize_image = crnn_resize_image(source_image, wh_ratio);
  resize_image.convertTo(resize_image, CV_32FC3, 1 / 255.f);

  const float *destination_image = reinterpret_cast<const float *>(resize_image.data);
  std::vector<float> data(resize_image.rows * resize_image.cols * 3);
  NHWC3ToNC3HW(destination_image, data.data(), resize_image.rows * resize_image.cols, mean, scale);

  ImageRaw image_raw {.data = data, .width = resize_image.cols, .height = resize_image.rows, .channels = 3};

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

cv::Mat crnn_resize_image(cv::Mat image, float wh_ratio) {
  int image_channels, image_height, image_width;
  image_channels = recognition_image_shape[0];
  image_height = recognition_image_shape[1];
  image_width = recognition_image_shape[2];

  image_width = int(32 * wh_ratio);
  // image_width = int(48 * wh_ratio);

  float ratio = static_cast<float>(image.cols) / static_cast<float>(image.rows);
  int resize_width, resize_height;
  if (ceilf(image_height * ratio) > image_width)
    resize_width = image_width;
  else
    resize_width = static_cast<int>(ceilf(image_height * ratio));
  cv::Mat resize_image;
  cv::resize(image, resize_image, cv::Size(resize_width, image_height), 0.f, 0.f, cv::INTER_LINEAR);

  return resize_image;
}

template <class ForwardIterator>
inline size_t argmax(ForwardIterator first, ForwardIterator last) {
  return std::distance(first, std::max_element(first, last));
}