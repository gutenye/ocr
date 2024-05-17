#pragma once

#include <any>
#include <string>
#include <unordered_map>

struct OptionModels {
  std::string detection_model_path {};
  std::string recognition_model_path {};
  std::string classifier_model_path {};
  std::string dictionary_path {};
};

struct Options {
  bool is_debug {false};
  double recognition_image_max_size {960};  // -1 to disable resize
  double detection_threshold {0.3};
  double detection_box_threshold {0.5};
  double detection_unclip_ratiop {1.6};
  bool detection_use_dilate {false};
  bool detection_use_polygon_score {true};
  bool detection_use_direction_classify {true};
  std::string output_dir {};
  OptionModels models {};
};

Options convertRawOptions(std::unordered_map<std::string, std::any>& rawOptions);