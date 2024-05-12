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

#include "native-ocr.h"
#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include "timer.h"

std::vector<std::string> ReadDict(std::string path);
cv::Mat GetRotateCropImage(cv::Mat srcimage, std::vector<std::vector<int>> box);
Options convertRawOptions(RawOptions rawOptions);

NativeOcr::NativeOcr(RawOptions rawOptions) : m_options {convertRawOptions(rawOptions)} {
  auto cPUThreadNum = 1;
  auto cPUPowerMode = "LITE_POWER_HIGH";
  try {
    // m_cls_predictor.reset(
    //     new ClsPredictor(clsModelDir, cPUThreadNum, cPUPowerMode));
    m_det_predictor.reset(new DetPredictor(m_options, cPUThreadNum, cPUPowerMode));
    m_rec_predictor.reset(new RecPredictor(m_options, cPUThreadNum, cPUPowerMode));
    m_dictionary = ReadDict(m_options.dictionary_path);
    m_dictionary.insert(m_dictionary.begin(), "#");
    m_dictionary.push_back(" ");
  } catch (std::string &error) {
    std::cerr << error << std::endl;
  }
}

std::vector<std::string> NativeOcr::Process(std::string &image_path) {
  try {
    Timer tic;
    tic.start();

    if (m_options.is_debug) {
      std::cout << "[DEBUG] Start Detection" << std::endl;
    }
    auto img = cv::imread(image_path);
    cv::Mat srcimg;
    img.copyTo(srcimg);

    auto detection_result = m_det_predictor->Predict(srcimg);

    if (m_options.is_debug) {
      std::cout << "[DEBUG] Start Recognition" << std::endl;
    }

    cv::Mat img_copy;
    img.copyTo(img_copy);
    cv::Mat crop_img;

    std::vector<std::string> rec_text;
    std::vector<float> rec_text_score;
    std::vector<RecognitionResult> recognitionResults;
    for (int i = detection_result.data.size() - 1; i >= 0; i--) {
      crop_img = GetRotateCropImage(img_copy, detection_result.data[i]);

      // if (m_options.detection_use_direction_classify)
      // {
      //   crop_img =
      //       m_cls_predictor->Predict(crop_img, nullptr, nullptr, nullptr, 0.9);
      // }
      auto recognitionResult = m_rec_predictor->Predict(crop_img, m_dictionary);
      recognitionResults.push_back(recognitionResult);
      rec_text.push_back(recognitionResult.data.first);
      rec_text_score.push_back(recognitionResult.data.second);
    }

    //// visualization
    // DEBUG
    // auto img_vis = Visualization(img, boxes, output_img_path);

    tic.end();
    auto total_time = tic.get_average_ms();
    if (m_options.is_debug) {
      float recognitionTotalSum = std::accumulate(
          recognitionResults.begin(), recognitionResults.end(), 0.0f,
          [](float accum, const RecognitionResult &result) { return accum + result.performance.total_time; });
      float recognitionPreprocessSum = std::accumulate(
          recognitionResults.begin(), recognitionResults.end(), 0.0f,
          [](float accum, const RecognitionResult &result) { return accum + result.performance.preprocess_time; });
      float recognitionPredictSum = std::accumulate(
          recognitionResults.begin(), recognitionResults.end(), 0.0f,
          [](float accum, const RecognitionResult &result) { return accum + result.performance.predict_time; });
      float recognitionPostprocessSum = std::accumulate(
          recognitionResults.begin(), recognitionResults.end(), 0.0f,
          [](float accum, const RecognitionResult &result) { return accum + result.performance.postprocess_time; });
      std::cout << "[DEBUG] Recognition costs " << (int)recognitionTotalSum << "ms "
                << "(preprocess:" << (int)(recognitionPreprocessSum / recognitionResults.size())
                << " predict:" << (int)(recognitionPredictSum / recognitionResults.size())
                << " preprocess:" << (int)(recognitionPreprocessSum / recognitionResults.size()) << ") avg * "
                << recognitionResults.size() << std::endl;
      std::cout << "[DEBUG] Detection costs " << (int)detection_result.performance.total_time << "ms "
                << "(preprocess:" << (int)detection_result.performance.preprocess_time
                << " predict:" << (int)detection_result.performance.predict_time
                << " postprocess:" << (int)detection_result.performance.postprocess_time << ")" << std::endl;
      std::cout << "[DEBUG] Total costs " << (int)total_time << "ms" << std::endl;
    }

    // print recognized text
    std::vector<std::string> lines(rec_text.size());
    for (int i = 0; i < lines.size(); i++) {
      if (m_options.is_debug) {
        std::cout << "[DEBUG] " << i << "\t" << rec_text_score[i] << "\t" << rec_text[i] << std::endl;
      }
      lines[i] = rec_text[i];
    }

    return lines;
  } catch (std::string &error) {
    std::cerr << error << std::endl;
    return std::vector<std::string> {};
  }
}

cv::Mat GetRotateCropImage(cv::Mat srcimage, std::vector<std::vector<int>> box) {
  cv::Mat image;
  srcimage.copyTo(image);
  std::vector<std::vector<int>> points = box;

  int x_collect[4] = {box[0][0], box[1][0], box[2][0], box[3][0]};
  int y_collect[4] = {box[0][1], box[1][1], box[2][1], box[3][1]};
  int left = int(*std::min_element(x_collect, x_collect + 4));
  int right = int(*std::max_element(x_collect, x_collect + 4));
  int top = int(*std::min_element(y_collect, y_collect + 4));
  int bottom = int(*std::max_element(y_collect, y_collect + 4));

  cv::Mat img_crop;
  image(cv::Rect(left, top, right - left, bottom - top)).copyTo(img_crop);

  for (int i = 0; i < points.size(); i++) {
    points[i][0] -= left;
    points[i][1] -= top;
  }

  int img_crop_width =
      static_cast<int>(sqrt(pow(points[0][0] - points[1][0], 2) + pow(points[0][1] - points[1][1], 2)));
  int img_crop_height =
      static_cast<int>(sqrt(pow(points[0][0] - points[3][0], 2) + pow(points[0][1] - points[3][1], 2)));

  cv::Point2f pts_std[4];
  pts_std[0] = cv::Point2f(0., 0.);
  pts_std[1] = cv::Point2f(img_crop_width, 0.);
  pts_std[2] = cv::Point2f(img_crop_width, img_crop_height);
  pts_std[3] = cv::Point2f(0.f, img_crop_height);

  cv::Point2f pointsf[4];
  pointsf[0] = cv::Point2f(points[0][0], points[0][1]);
  pointsf[1] = cv::Point2f(points[1][0], points[1][1]);
  pointsf[2] = cv::Point2f(points[2][0], points[2][1]);
  pointsf[3] = cv::Point2f(points[3][0], points[3][1]);

  cv::Mat M = cv::getPerspectiveTransform(pointsf, pts_std);

  cv::Mat dst_img;
  cv::warpPerspective(img_crop, dst_img, M, cv::Size(img_crop_width, img_crop_height), cv::BORDER_REPLICATE);

  const float ratio = 1.5;
  if (static_cast<float>(dst_img.rows) >= static_cast<float>(dst_img.cols) * ratio) {
    cv::Mat srcCopy = cv::Mat(dst_img.rows, dst_img.cols, dst_img.depth());
    cv::transpose(dst_img, srcCopy);
    cv::flip(srcCopy, srcCopy, 0);
    return srcCopy;
  } else {
    return dst_img;
  }
}

std::vector<std::string> ReadDict(std::string path) {
  std::ifstream in(path);
  std::string filename;
  std::string line;
  std::vector<std::string> m_vec;
  if (!in) {
    throw std::string("Error: ReadDict file not found '") + path + "'";
  }
  while (getline(in, line)) {
    m_vec.push_back(line);
  }
  return m_vec;
}

std::vector<std::string> split(const std::string &str, const std::string &delim) {
  std::vector<std::string> res;
  if ("" == str) return res;
  char *strs = new char[str.length() + 1];
  std::strcpy(strs, str.c_str());

  char *d = new char[delim.length() + 1];
  std::strcpy(d, delim.c_str());

  char *p = std::strtok(strs, d);
  while (p) {
    std::string s = p;
    res.push_back(s);
    p = std::strtok(NULL, d);
  }

  return res;
}

cv::Mat Visualization(cv::Mat srcimg, std::vector<std::vector<std::vector<int>>> boxes, std::string output_image_path) {
  cv::Point rook_points[boxes.size()][4];
  for (int n = 0; n < boxes.size(); n++) {
    for (int m = 0; m < boxes[0].size(); m++) {
      rook_points[n][m] = cv::Point(static_cast<int>(boxes[n][m][0]), static_cast<int>(boxes[n][m][1]));
    }
  }
  cv::Mat img_vis;
  srcimg.copyTo(img_vis);
  for (int n = 0; n < boxes.size(); n++) {
    const cv::Point *ppt[1] = {rook_points[n]};
    int npt[] = {4};
    cv::polylines(img_vis, ppt, npt, 1, 1, CV_RGB(0, 255, 0), 2, 8, 0);
  }

  cv::imwrite(output_image_path, img_vis);
  std::cout << "The detection visualized image saved in " << output_image_path.c_str() << std::endl;
  return img_vis;
}

Options convertRawOptions(RawOptions rawOptions) {
  Options options {};
  if (rawOptions.count("isDebug") > 0) {
    options.is_debug = std::get<bool>(rawOptions["isDebug"]);
  }
  if (rawOptions.count("imageMaxSize") > 0) {
    options.image_max_size = std::get<double>(rawOptions.at("imageMaxSize"));
  }
  if (rawOptions.count("detectionThreshold") > 0) {
    options.detection_threshold = std::get<double>(rawOptions.at("detectionThreshold"));
  }
  if (rawOptions.count("detectionBoxThreshold") > 0) {
    options.detection_box_threshold = std::get<double>(rawOptions.at("detectionBoxThreshold"));
  }
  if (rawOptions.count("detectionUnclipRatiop") > 0) {
    options.detection_unclip_ratiop = std::get<double>(rawOptions.at("detectionUnclipRatiop"));
  }
  if (rawOptions.count("detectionUseDilate") > 0) {
    options.detection_use_dilate = std::get<bool>(rawOptions.at("detectionUseDilate"));
  }
  if (rawOptions.count("detectionUsePolygonScore") > 0) {
    options.detection_use_polygon_score = std::get<bool>(rawOptions.at("detectionUsePolygonScore"));
  }
  if (rawOptions.count("detectionuseDirectionClassify") > 0) {
    options.detection_use_direction_classify = std::get<bool>(rawOptions.at("detectionuseDirectionClassify"));
  }
  if (rawOptions.count("detectionModelPath") > 0) {
    options.detection_model_path = std::get<std::string>(rawOptions.at("detectionModelPath"));
  }
  if (rawOptions.count("recognitionModelPath") > 0) {
    options.recognition_model_path = std::get<std::string>(rawOptions.at("recognitionModelPath"));
  }
  if (rawOptions.count("classififerModelPath") > 0) {
    options.classifier_model_path = std::get<std::string>(rawOptions.at("classififerModelPath"));
  }
  if (rawOptions.count("dictionaryPath") > 0) {
    options.dictionary_path = std::get<std::string>(rawOptions.at("dictionaryPath"));
  }
  return options;
}