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

#include "onnx.h"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "options.h"
#include "shared.h"
#include "utils.h"

using ClassifierResultData = cv::Mat;

struct ClassifierResult {
  ClassifierResultData data {};
  ModelPerformance performance {};
};

class ClassifierPredictor {
public:
  explicit ClassifierPredictor(Options &options);

  ClassifierResult predict(const cv::Mat &image, const float thresh);

private:
  Options m_options {};

  Onnx m_onnx;

  ImageRaw preprocess(const cv::Mat &image);

  ClassifierResultData postprocess(ModelOutput &model_output, const cv::Mat &source_image, const float thresh);
};
