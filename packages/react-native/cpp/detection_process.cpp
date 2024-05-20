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
#include <cmath>
#include <memory>
#include <utility>
#include "db_post_process.h"
#include "timer.h"

void resize_image(const cv::Mat image, cv::Mat &resized_image, Options &options);

DetectionPredictor::DetectionPredictor(Options &options)
    : m_options {options}, m_onnx {Onnx(options.models.detection_model_path)} {}

DetectionResult DetectionPredictor::predict(cv::Mat &image) {
  ModelPerformance performance {};

  cv::Mat source_image;
  image.copyTo(source_image);

  Timer timer;
  timer.start();
  auto preprocess_result = preprocess(image);
  timer.end();
  performance.preprocess_time = timer.get_average_ms();

  auto &model_input = preprocess_result.model_input;
  auto &resized_image = preprocess_result.resized_image;

  // Run predictor
  std::vector<int64_t> input_shape = {1, model_input.channels, model_input.height, model_input.width};
  timer.start();
  auto model_output = m_onnx.run(model_input.data, input_shape);
  timer.end();
  performance.predict_time = timer.get_average_ms();

  // Process Output
  timer.start();
  auto filter_boxes = postprocess(model_output, source_image, resized_image, m_options);
  timer.end();
  performance.postprocess_time = timer.get_average_ms();

  performance.total_time = performance.preprocess_time + performance.predict_time + performance.postprocess_time;

  return DetectionResult {.data = filter_boxes, .performance = performance};
}

DetectionPreprocessResult DetectionPredictor::preprocess(const cv::Mat &source_image) {
  DetectionPreprocessResult result {};
  resize_image(source_image, result.resized_image, m_options);

  cv::Mat model_data;
  result.resized_image.convertTo(model_data, CV_32FC3, 1.0 / 255.f);

  std::vector<float> data(model_data.rows * model_data.cols * 3);
  std::vector<float> mean = {0.485f, 0.456f, 0.406f};
  std::vector<float> scale = {1 / 0.229f, 1 / 0.224f, 1 / 0.225f};
  const float *destination_image = reinterpret_cast<const float *>(model_data.data);
  NHWC3ToNC3HW(destination_image, data.data(), model_data.rows * model_data.cols, mean, scale);

  result.model_input = ImageRaw {.data = data, .width = model_data.cols, .height = model_data.rows, .channels = 3};

  return result;
}

DetectionResultData DetectionPredictor::postprocess(ModelOutput &model_output, const cv::Mat &source_image,
                                                    const cv::Mat &resized_image, Options &options) {
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

  auto ratio_h = static_cast<float>(resized_image.rows) / static_cast<float>(source_image.rows);
  auto ratio_w = static_cast<float>(resized_image.cols) / static_cast<float>(source_image.cols);
  std::vector<std::vector<std::vector<int>>> filter_boxes = filter_tag_det_res(boxes, ratio_h, ratio_w, source_image);

  return filter_boxes;
}

// resize image to a size multiple of 32 which is required by the network
void resize_image(const cv::Mat image, cv::Mat &resized_image, Options &options) {
  int width = image.cols;
  int height = image.rows;
  float ratio = 1.f;

  int max_wh = width >= height ? width : height;
  auto max_size = options.recognition_image_max_size;
  if (max_size != -1 && max_wh > max_size) {
    if (height > width) {
      ratio = static_cast<float>(max_size) / static_cast<float>(height);
    } else {
      ratio = static_cast<float>(max_size) / static_cast<float>(width);
    }
  }

  int resize_height = static_cast<int>(float(height) * ratio);
  int resize_width = static_cast<int>(float(width) * ratio);
  if (resize_height % 32 == 0) {
    resize_height = resize_height;
  } else if (resize_height / 32 < 1 + 1e-5) {
    resize_height = 32;
  } else {
    // TODO
    // resize_height = (resize_height / 32 - 1) * 32;
    resize_height = ceil(static_cast<double>(resize_height) / 32) * 32;
  }
  if (resize_width % 32 == 0) {
    resize_width = resize_width;
  } else if (resize_width / 32 < 1 + 1e-5) {
    resize_width = 32;
  } else {
    // resize_width = (resize_width / 32 - 1) * 32;
    resize_width = ceil(static_cast<double>(resize_width) / 32) * 32;
  }

  if (options.is_debug) {
    std::cout << "[DEBUG] Resize image from " << width << "x" << height << " to " << resize_width << "x"
              << resize_height << std::endl;
  }

  cv::resize(image, resized_image, cv::Size(resize_width, resize_height));
}