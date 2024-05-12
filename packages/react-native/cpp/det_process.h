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

#include <map>
#include "onnx.h"
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "shared.h"
#include "utils.h"

class DetPredictor {
public:
  explicit DetPredictor(Options &options, const int cpuThreadNum, const std::string &cpuPowerMode);

  std::vector<std::vector<std::vector<int>>> Predict(cv::Mat &rgbImage);

private:
  Options m_options {};
  std::vector<float> ratio_hw_;

  ImageRaw Preprocess(const cv::Mat &img, const int max_side_len);

  std::vector<std::vector<std::vector<int>>> Postprocess(ModelOutput &model_output, const cv::Mat &srcimg,
                                                         Options &options);
};
