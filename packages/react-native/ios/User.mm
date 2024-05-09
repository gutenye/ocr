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
// clang-format off
#import <opencv2/opencv.hpp>
#import <opencv2/imgcodecs/ios.h>
#import <opencv2/videoio/cap_ios.h>
// clang-format on
#import "User.h"
#include "pipeline.h"
#include "timer.h"
#include <arm_neon.h>
#include <iostream>
#include <mutex>
#include <paddle_api.h>
#include <paddle_use_kernels.h>
#include <paddle_use_ops.h>
#include <string>
#import <sys/timeb.h>
#include <vector>

using namespace paddle::lite_api;
using namespace cv;

Pipeline *pipe_;

@implementation User
- (void) run {
  NSString *path = [[NSBundle mainBundle] bundlePath];
  std::string paddle_dir = std::string([path UTF8String]);
  std::string det_model_file = paddle_dir + "/models/ch_PP-OCRv4_det_infer.onnx";
  std::string rec_model_file = paddle_dir + "/models/ch_PP-OCRv4_rec_infer.onnx";
  std::string cls_model_file = paddle_dir + "/models/ch_ppocr_mobile_v2.0_cls_infer.onnx";
  std::string img_path = paddle_dir + "/models/cn-01.jpg";
  std::string output_img_path = paddle_dir + "/test_result.jpg";
  std::string dict_path = paddle_dir + "/models/ppocr_keys_v1.txt";
  std::string config_path = paddle_dir + "/models/config.txt";

  cv::Mat srcimg = imread(img_path);
  pipe_ = new Pipeline(det_model_file, cls_model_file, rec_model_file,
                       "LITE_POWER_HIGH", 1, config_path, dict_path);
  std::ostringstream result;
  std::vector<std::string> res_txt;
  cv::Mat img_vis = pipe_->Process(srcimg, output_img_path, res_txt);

  // print result
  for (int i = 0; i < res_txt.size() / 2; i++) {
      result << i << "\t" << res_txt[2*i] << "\t" << res_txt[2*i + 1] <<
      "\n";
  }

  std::string output = result.str();
  std::cout << output << std::endl;
}

@end
