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

#include <any>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include "classifier_process.h"
#include "detection_process.h"
#include "recognition_process.h"
#include "shared.h"

class NativeOcr {
public:
  NativeOcr(std::unordered_map<std::string, std::any> rawOptions, const std::string &assetDir,
            const std::string &debugOutputDir);

  std::vector<std::string> detect(std::string &image_path);

private:
  Options m_options;
  std::vector<std::string> m_dictionary;
  std::shared_ptr<ClassifierPredictor> m_classifier_predictor;
  std::shared_ptr<DetectionPredictor> m_detection_predictor;
  std::shared_ptr<RecognitionPredictor> m_recognition_predictor;
};
