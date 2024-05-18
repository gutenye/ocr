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

#pragma once
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "utils.h"

class ClassifierPredictor {
public:
  explicit ClassifierPredictor(const std::string &modelDir, const int cpu_thread_num,
                               const std::string &cpu_power_mode);

  cv::Mat predict(const cv::Mat &rgb_image, double *preprocess_time, double *predictTime, double *postprocessTime,
                  const float thresh);

private:
  void preprocess(const cv::Mat &rgba_image);
  cv::Mat postprocess(const cv::Mat &img, const float thresh);
};
