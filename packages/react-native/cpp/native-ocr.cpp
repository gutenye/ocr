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
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include "timer.h"

namespace fs = std::filesystem;

std::vector<std::string> read_dictionary(std::string path);
CropResult get_rotate_crop_image(cv::Mat &source_image, std::vector<std::vector<int>> box);
void visualization(cv::Mat source_image, std::vector<std::vector<std::vector<int>>> boxes,
                   std::string output_image_path);

NativeOcr::NativeOcr(std::unordered_map<std::string, std::any> rawOptions, const std::string &assetDir,
                     const std::string &debugOutputDir)
    : m_options {convertRawOptions(rawOptions, assetDir, debugOutputDir)} {
  m_classifier_predictor.reset(new ClassifierPredictor(m_options));
  m_detection_predictor.reset(new DetectionPredictor(m_options));
  m_recognition_predictor.reset(new RecognitionPredictor(m_options));
  m_dictionary = read_dictionary(m_options.models.dictionary_path);
  m_dictionary.insert(m_dictionary.begin(), "#");
  m_dictionary.push_back(" ");
}

std::vector<TextLine> NativeOcr::detect(std::string &image_path) {
  Timer timer;
  timer.start();

  if (!fs::exists(image_path)) {
    throw std::runtime_error("Image file does not exist - '" + image_path + "'");
  }

  if (m_options.is_debug) {
    std::cout << "[DEBUG] Start detection" << std::endl;
  }
  auto source_image = cv::imread(image_path);

  auto detection_result = m_detection_predictor->predict(source_image);

  if (m_options.is_debug) {
    auto output_path = m_options.debug_output_dir + "/boxes.jpg";
    visualization(source_image, detection_result.data, output_path);
    std::cout << "[DEBUG] Detection visualized image saved in " << output_path << std::endl;
  }

  if (m_options.is_debug) {
    std::cout << "[DEBUG] Start recognition" << std::endl;
  }

  std::vector<TextLine> text_lines;
  std::vector<ClassifierResult> classifier_results;
  std::vector<RecognitionResult> recognition_results;
  for (int i = detection_result.data.size() - 1; i >= 0; i--) {
    auto box = detection_result.data[i];
    // box: [p1, p2, p3, p4], p: [x,y]
    auto crop = get_rotate_crop_image(source_image, box);

    if (m_options.is_debug) {
      // auto output_path =
      //     m_options.debug_output_dir + "/line-" + std::to_string(detection_result.data.size() - 1 - i) + ".jpg";
      // cv::imwrite(output_path, crop.image);
    }

    if (m_options.use_direction_classify) {
      auto threshold = 0.9;
      auto classifier_result = m_classifier_predictor->predict(crop.image, threshold);
      classifier_results.push_back(classifier_result);
      crop.image = classifier_result.data;
      if (m_options.is_debug) {
        auto output_path = m_options.debug_output_dir + "/line-" +
                           std::to_string(detection_result.data.size() - 1 - i) + "-direction.jpg";
        cv::imwrite(output_path, crop.image);
      }
    }

    cv::Mat resized_image;
    auto recognition_result = m_recognition_predictor->predict(crop.image, crop.frame, m_dictionary, resized_image);
    recognition_results.push_back(recognition_result);
    text_lines.push_back(recognition_result.data);

    // if (m_options.is_debug) {
    //   auto output_path =
    //       m_options.debug_output_dir + "/line-" + std::to_string(detection_result.data.size() - 1 - i) +
    //       "-resized.jpg";
    //   cv::imwrite(output_path, resized_image);
    // }
  }

  timer.end();
  auto total_time = timer.get_average_ms();
  if (m_options.is_debug) {
    float classifier_total_sum = std::accumulate(
        classifier_results.begin(), classifier_results.end(), 0.0f,
        [](float accumulator, const ClassifierResult &result) { return accumulator + result.performance.total_time; });
    float classifier_preprocess_sum = std::accumulate(classifier_results.begin(), classifier_results.end(), 0.0f,
                                                      [](float accumulator, const ClassifierResult &result) {
                                                        return accumulator + result.performance.preprocess_time;
                                                      });
    float classifier_predict_sum = std::accumulate(classifier_results.begin(), classifier_results.end(), 0.0f,
                                                   [](float accumulator, const ClassifierResult &result) {
                                                     return accumulator + result.performance.predict_time;
                                                   });
    float classifier_postproces_sum = std::accumulate(classifier_results.begin(), classifier_results.end(), 0.0f,
                                                      [](float accumulator, const ClassifierResult &result) {
                                                        return accumulator + result.performance.postprocess_time;
                                                      });

    float recognition_total_sum = std::accumulate(
        recognition_results.begin(), recognition_results.end(), 0.0f,
        [](float accumulator, const RecognitionResult &result) { return accumulator + result.performance.total_time; });
    float recognition_preprocess_sum = std::accumulate(recognition_results.begin(), recognition_results.end(), 0.0f,
                                                       [](float accumulator, const RecognitionResult &result) {
                                                         return accumulator + result.performance.preprocess_time;
                                                       });
    float recognition_predict_sum = std::accumulate(recognition_results.begin(), recognition_results.end(), 0.0f,
                                                    [](float accumulator, const RecognitionResult &result) {
                                                      return accumulator + result.performance.predict_time;
                                                    });
    float recognition_postproces_sum = std::accumulate(recognition_results.begin(), recognition_results.end(), 0.0f,
                                                       [](float accumulator, const RecognitionResult &result) {
                                                         return accumulator + result.performance.postprocess_time;
                                                       });
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "[DEBUG] Detection costs " << (int)detection_result.performance.total_time << "ms "
              << "(preprocess:" << (int)detection_result.performance.preprocess_time
              << " predict:" << (int)detection_result.performance.predict_time
              << " postprocess:" << (int)detection_result.performance.postprocess_time << ")" << std::endl;
    std::cout << "[DEBUG] Recognition costs " << (int)recognition_total_sum << "ms "
              << "(preprocess:" << (float)(recognition_preprocess_sum / recognition_results.size())
              << " predict:" << (int)(recognition_predict_sum / recognition_results.size())
              << " postprocess:" << (float)(recognition_preprocess_sum / recognition_results.size()) << ") avg * "
              << recognition_results.size() << std::endl;
    std::cout << "[DEBUG] Total costs " << (int)total_time << "ms" << std::endl;
  }

  // print recognized text
  if (m_options.is_debug) {
    for (size_t index = 0; index < text_lines.size(); index++) {
      auto text_line = text_lines[index];
      std::cout << "[DEBUG] " << index << "\t" << text_line.score << "\t" << text_line.text << "\t" << text_line.frame
                << std::endl;
    }
  }

  return text_lines;
}

CropResult get_rotate_crop_image(cv::Mat &source_image, std::vector<std::vector<int>> box) {
  cv::Mat image;
  source_image.copyTo(image);
  std::vector<std::vector<int>> points = box;

  int x_collect[4] = {box[0][0], box[1][0], box[2][0], box[3][0]};
  int y_collect[4] = {box[0][1], box[1][1], box[2][1], box[3][1]};
  int left = int(*std::min_element(x_collect, x_collect + 4));
  int right = int(*std::max_element(x_collect, x_collect + 4));
  int top = int(*std::min_element(y_collect, y_collect + 4));
  int bottom = int(*std::max_element(y_collect, y_collect + 4));
  int width = right - left;
  int height = bottom - top;
  Frame frame {
      .top = top,
      .left = left,
      .width = width,
      .height = height,
  };

  cv::Mat img_crop;
  image(cv::Rect(left, top, width, height)).copyTo(img_crop);

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
    return CropResult {.image = srcCopy, .frame = frame};
  } else {
    return CropResult {.image = dst_img, .frame = frame};
  }
}

std::vector<std::string> read_dictionary(std::string path) {
  std::ifstream in(path);
  std::string filename;
  std::string line;
  std::vector<std::string> m_vec;
  if (!in) {
    throw std::runtime_error("read_dictionary file not found '" + path + "'");
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

void visualization(cv::Mat source_image, std::vector<std::vector<std::vector<int>>> boxes,
                   std::string output_image_path) {
  cv::Point rook_points[boxes.size()][4];
  for (int n = 0; n < boxes.size(); n++) {
    for (int m = 0; m < boxes[0].size(); m++) {
      rook_points[n][m] = cv::Point(static_cast<int>(boxes[n][m][0]), static_cast<int>(boxes[n][m][1]));
    }
  }
  cv::Mat image_visualization;
  source_image.copyTo(image_visualization);
  for (int n = 0; n < boxes.size(); n++) {
    const cv::Point *ppt[1] = {rook_points[n]};
    int npt[] = {4};
    cv::polylines(image_visualization, ppt, npt, 1, 1, CV_RGB(255, 0, 0), 1, 8, 0);
  }

  cv::imwrite(output_image_path, image_visualization);
}
