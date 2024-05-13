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

#include "detection_process.h"
#include <format>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "db_post_process.h"
#include "timer.h"

cv::Mat resize_image(const cv::Mat image, int image_max_size, std::vector<float> &ratio_hw);

DetectionPredictor::DetectionPredictor(Options &options, const int cpu_thread_num, const std::string &cpu_power_mode)
    : m_options {options}, m_onnx {Onnx(options.detection_model_path)} {}

DetectionResult DetectionPredictor::predict(cv::Mat &image) {
  ModelPerformance performance;

  cv::Mat source_image;
  image.copyTo(source_image);

  Timer timer;
  timer.start();
  auto input = preprocess(image, m_options.recognition_image_max_size);
  timer.end();
  performance.preprocess_time = timer.get_average_ms();

  // Run predictor
  std::vector<int64_t> input_shape = {1, input.channels, input.height, input.width};

  timer.start();
  auto model_output = m_onnx.run(input.data, input_shape);
  timer.end();
  performance.predict_time = timer.get_average_ms();

  // Process Output
  timer.start();
  auto filter_boxes = postprocess(model_output, source_image, m_options);
  timer.end();
  performance.postprocess_time = timer.get_average_ms();

  performance.total_time = performance.preprocess_time + performance.predict_time + performance.postprocess_time;

  return DetectionResult {.data = filter_boxes, .performance = performance};
}

ImageRaw DetectionPredictor::preprocess(const cv::Mat &source_image, const int image_max_size) {
  cv::Mat image = resize_image(source_image, image_max_size, m_ratio_hw);
  cv::Mat image_fp;
  image.convertTo(image_fp, CV_32FC3, 1.0 / 255.f);

  std::vector<float> data(image_fp.rows * image_fp.cols * 3);
  std::vector<float> mean = {0.485f, 0.456f, 0.406f};
  std::vector<float> scale = {1 / 0.229f, 1 / 0.224f, 1 / 0.225f};
  const float *destination_image = reinterpret_cast<const float *>(image_fp.data);
  NHWC3ToNC3HW(destination_image, data.data(), image_fp.rows * image_fp.cols, mean, scale);

  ImageRaw image_raw {.data = data, .width = image_fp.cols, .height = image_fp.rows, .channels = 3};

  return image_raw;
}

DetectionResultData DetectionPredictor::postprocess(ModelOutput &model_output, const cv::Mat &source_image,
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

  std::vector<std::vector<std::vector<int>>> filter_boxes =
      FilterTagDetRes(boxes, m_ratio_hw[0], m_ratio_hw[1], source_image);

  return filter_boxes;
}

// resize image to a size multiple of 32 which is required by the network
cv::Mat resize_image(const cv::Mat image, int image_max_size, std::vector<float> &ratio_hw) {
  int width = image.cols;
  int height = image.rows;
  float ratio = 1.f;
  int max_wh = width >= height ? width : height;
  if (max_wh > image_max_size) {
    if (height > width) {
      ratio = static_cast<float>(image_max_size) / static_cast<float>(height);
    } else {
      ratio = static_cast<float>(image_max_size) / static_cast<float>(width);
    }
  }

  int resize_h = static_cast<int>(float(height) * ratio);
  int resize_w = static_cast<int>(float(width) * ratio);
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

  cv::resize(image, resize_img, cv::Size(resize_w, resize_h));

  ratio_hw.push_back(static_cast<float>(resize_h) / static_cast<float>(height));
  ratio_hw.push_back(static_cast<float>(resize_w) / static_cast<float>(width));
  return resize_img;
}