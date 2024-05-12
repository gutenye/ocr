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
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <variant>
#include <vector>
#include "det_process.h"
#include "rec_process.h"
#include "shared.h"

using RawOptions = std::unordered_map<std::string, std::variant<bool, double, std::string>>;

class NativeOcr {
public:
  NativeOcr(RawOptions rawOptions);

  std::vector<std::string> Process(std::string &image_path);

private:
  Options m_options;
  // TODO: charactor_dict_ -> m_dictionary
  std::vector<std::string> charactor_dict_;
  // std::shared_ptr<ClsPredictor> clsPredictor_;
  std::shared_ptr<DetPredictor> detPredictor_;
  std::shared_ptr<RecPredictor> recPredictor_;
};
