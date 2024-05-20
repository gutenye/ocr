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

#include "classifier_process.h"
#include <iostream>
#include "timer.h"

const std::vector<int> classifier_image_shape {3, 48, 192};

cv::Mat classifier_resize_image(cv::Mat source_image, int resize_height, int max_resize_width, Options &options);

ClassifierPredictor::ClassifierPredictor(Options &options)
    : m_options {options}, m_onnx {Onnx(options.models.classifier_model_path)} {}

ClassifierResult ClassifierPredictor::predict(const cv::Mat &source_image, const float thresh) {
  ModelPerformance performance {};

  Timer timer;
  timer.start();
  auto image = preprocess(source_image);
  timer.end();
  performance.preprocess_time = timer.get_average_ms();

  // Run predictor
  std::vector<int64_t> input_shape {1, image.channels, image.height, image.width};
  timer.start();
  // TODO: hangs on run
  std::cout << "[DEBUG] classifier run start" << std::endl;
  auto model_output = m_onnx.run(image.data, input_shape);
  std::cout << "[DEBUG] classifier run end" << std::endl;
  timer.end();
  performance.predict_time = timer.get_average_ms();

  // Process Output
  timer.start();
  std::cout << "[DEBUG] 0" << std::endl;
  auto result_image = postprocess(model_output, source_image, thresh);
  timer.end();
  performance.postprocess_time = timer.get_average_ms();

  performance.total_time = performance.preprocess_time + performance.predict_time + performance.postprocess_time;

  return ClassifierResult {.data = result_image, .performance = performance};
}

ImageRaw ClassifierPredictor::preprocess(const cv::Mat &source_image) {
  std::vector<float> mean = {0.5f, 0.5f, 0.5f};
  std::vector<float> scale = {1 / 0.5f, 1 / 0.5f, 1 / 0.5f};

  cv::Mat resized_image =
      classifier_resize_image(source_image, classifier_image_shape[1], classifier_image_shape[2], m_options);
  resized_image.convertTo(resized_image, CV_32FC3, 1 / 255.f);

  const float *destination_image = reinterpret_cast<const float *>(resized_image.data);
  std::vector<float> data(resized_image.rows * resized_image.cols * 3);
  NHWC3ToNC3HW(destination_image, data.data(), resized_image.rows * resized_image.cols, mean, scale);

  ImageRaw image_raw {.data = data, .width = resized_image.cols, .height = resized_image.rows, .channels = 3};

  return image_raw;
}

ClassifierResultData ClassifierPredictor::postprocess(ModelOutput &model_output, const cv::Mat &source_image,
                                                      const float thresh) {
  std::cout << "[DEBUG] 1" << std::endl;
  auto softmax_scores = model_output.data;
  auto softmax_out_shape = model_output.shape;
  float score = 0;
  int label = 0;
  for (int i = 0; i < softmax_out_shape[1]; i++) {
    if (softmax_scores[i] > score) {
      score = softmax_scores[i];
      label = i;
    }
  }
  if (label % 2 == 1 && score > thresh) {
    std::cout << "[DEBUG] 2" << std::endl;
    cv::rotate(source_image, source_image, 1);
    std::cout << "[DEBUG] 3" << std::endl;
  }
  return source_image;
}

cv::Mat classifier_resize_image(cv::Mat source_image, int resize_height, int max_resize_width, Options &options) {
  auto source_width = source_image.cols;
  auto source_height = source_image.rows;
  float wh_ratio = static_cast<float>(source_width) / static_cast<float>(source_height);
  auto resize_width = int(ceilf(resize_height * wh_ratio));
  if (resize_width > max_resize_width) {
    resize_width = max_resize_width;
  }
  if (options.is_debug) {
    std::cout << "[DEBUG] Classifier resize image from " << source_width << "x" << source_height << " to "
              << resize_width << "x" << resize_height << std::endl;
  }
  cv::Mat resized_image;
  cv::resize(source_image, resized_image, cv::Size(resize_width, resize_height), 0.f, 0.f, cv::INTER_LINEAR);
  if (resize_width < max_resize_width) {
    cv::copyMakeBorder(resized_image, resized_image, 0, 0, 0, max_resize_width - resize_width, cv::BORDER_CONSTANT,
                       cv::Scalar(0, 0, 0));
  }
  return resized_image;
}
