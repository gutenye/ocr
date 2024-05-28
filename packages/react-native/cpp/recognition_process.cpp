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
#include <iostream>
#include "shared.h"
#include "timer.h"
#include "utils.h"

// TODO
// index: 2 Got: 32 Expected: 48
// const std::vector<int> recognition_image_shape {3, 32, 320};
const std::vector<int> recognition_image_shape {3, 48, 480};

cv::Mat recognition_resize_image(cv::Mat image, int resize_height, int resize_max_width, Options &options);
template <class ForwardIterator>
inline size_t argmax(ForwardIterator first, ForwardIterator last);

RecognitionPredictor::RecognitionPredictor(Options &options)
    : m_options {options}, m_onnx {Onnx {options.models.recognition_model_path}} {}

RecognitionResult RecognitionPredictor::predict(const cv::Mat &source_image, const Frame &frame,
                                                std::vector<std::string> charactor_dict, cv::Mat &resized_image) {
  ModelPerformance performance {};
  Timer timer;
  timer.start();
  auto image = preprocess(source_image, resized_image);
  timer.end();
  performance.preprocess_time = timer.get_average_ms();

  // Run predictor
  std::vector<int64_t> input_shape {1, image.channels, image.height, image.width};
  timer.start();
  auto model_output = m_onnx.run(image.data, input_shape);
  timer.end();
  performance.predict_time = timer.get_average_ms();

  timer.start();
  auto text_line = postprocess(model_output, frame, source_image, charactor_dict);
  timer.end();
  performance.postprocess_time = timer.get_average_ms();

  performance.total_time = performance.preprocess_time + performance.predict_time + performance.postprocess_time;

  return RecognitionResult {.data = text_line, .performance = performance};
}

ImageRaw RecognitionPredictor::preprocess(const cv::Mat &source_image, cv::Mat &resized_image) {
  std::vector<float> mean = {0.5f, 0.5f, 0.5f};
  std::vector<float> scale = {1 / 0.5f, 1 / 0.5f, 1 / 0.5f};

  resized_image =
      recognition_resize_image(source_image, recognition_image_shape[1], recognition_image_shape[2], m_options);

  cv::Mat model_data;
  resized_image.convertTo(model_data, CV_32FC3, 1 / 255.f);

  const float *destination_image = reinterpret_cast<const float *>(model_data.data);
  std::vector<float> data(model_data.rows * model_data.cols * 3);
  NHWC3ToNC3HW(destination_image, data.data(), model_data.rows * model_data.cols, mean, scale);

  ImageRaw image_raw {.data = data, .width = model_data.cols, .height = model_data.rows, .channels = 3};

  return image_raw;
}

TextLine RecognitionPredictor::postprocess(ModelOutput &model_output, const Frame &frame, const cv::Mat &source_image,
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

  return TextLine {
      .text = text,
      .score = score,
      .frame = frame,
  };
}

cv::Mat recognition_resize_image(cv::Mat source_image, int resize_height, int resize_max_width, Options &options) {
  auto source_width = source_image.cols;
  auto source_height = source_image.rows;
  float wh_ratio = static_cast<float>(source_width) / static_cast<float>(source_height);
  auto resize_width = int(ceilf(resize_height * wh_ratio));
  if (resize_width > resize_max_width) {
    resize_width = resize_max_width;
  }
  if (options.is_debug) {
    // std::cout << "[DEBUG] Recognition resize image from " << source_width << "x" << source_height << " to "
    //           << resize_width << "x" << resize_height << std::endl;
  }
  cv::Mat resized_image;
  cv::resize(source_image, resized_image, cv::Size(resize_width, resize_height), 0.f, 0.f, cv::INTER_LINEAR);
  return resized_image;
}

template <class ForwardIterator>
inline size_t argmax(ForwardIterator first, ForwardIterator last) {
  return std::distance(first, std::max_element(first, last));
}