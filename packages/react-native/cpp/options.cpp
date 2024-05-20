
#include "options.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

Options convertRawOptions(std::unordered_map<std::string, std::any>& rawOptions, const std::string& assetDir,
                          const std::string& debugOutputDir) {
  Options options {};
  if (rawOptions.count("isDebug") > 0) {
    options.is_debug = std::any_cast<bool>(rawOptions.at("isDebug"));
  }
  if (rawOptions.count("debugOutputDir") == 0) {
    rawOptions["debugOutputDir"] = debugOutputDir;
  }
  if (rawOptions.count("debugOutputDir") > 0) {
    options.debug_output_dir = std::any_cast<std::string>(rawOptions.at("debugOutputDir"));
    fs::create_directories(options.debug_output_dir);
  }
  if (rawOptions.count("recognitionImageMaxSize") > 0) {
    options.recognition_image_max_size = std::any_cast<double>(rawOptions.at("recognitionImageMaxSize"));
  }
  if (rawOptions.count("detectionThreshold") > 0) {
    options.detection_threshold = std::any_cast<double>(rawOptions.at("detectionThreshold"));
  }
  if (rawOptions.count("detectionBoxThreshold") > 0) {
    options.detection_box_threshold = std::any_cast<double>(rawOptions.at("detectionBoxThreshold"));
  }
  if (rawOptions.count("detectionUnclipRatiop") > 0) {
    options.detection_unclip_ratiop = std::any_cast<double>(rawOptions.at("detectionUnclipRatiop"));
  }
  if (rawOptions.count("detectionUseDilate") > 0) {
    options.detection_use_dilate = std::any_cast<bool>(rawOptions.at("detectionUseDilate"));
  }
  if (rawOptions.count("detectionUsePolygonScore") > 0) {
    options.detection_use_polygon_score = std::any_cast<bool>(rawOptions.at("detectionUsePolygonScore"));
  }
  if (rawOptions.count("detectionuseDirectionClassify") > 0) {
    options.detection_use_direction_classify = std::any_cast<bool>(rawOptions.at("detectionuseDirectionClassify"));
  }
  if (rawOptions.count("models") == 0) {
    rawOptions["models"] = std::unordered_map<std::string, std::any> {
        {"detectionModelPath", assetDir + "/ch_PP-OCRv4_det_infer.onnx"},
        {"recognitionModelPath", assetDir + "/ch_PP-OCRv4_rec_infer.onnx"},
        {"classifierModelPath", assetDir + "/ch_ppocr_mobile_v2.0_cls_infer.onnx"},
        {"dictionaryPath", assetDir + "/ppocr_keys_v1.txt"}};
  }
  auto rawModels = std::any_cast<std::unordered_map<std::string, std::any>>(rawOptions.at("models"));
  auto& models = options.models;
  if (rawModels.count("detectionModelPath") == 0) {
    throw std::runtime_error("Ocr.create options.models.detectionModelPath is required.");
  }
  models.detection_model_path = std::any_cast<std::string>(rawModels.at("detectionModelPath"));
  if (rawModels.count("recognitionModelPath") == 0) {
    throw std::runtime_error("Ocr.create options.models.recognitionModelPath is required.");
  }
  models.recognition_model_path = std::any_cast<std::string>(rawModels.at("recognitionModelPath"));
  if (rawModels.count("classifierModelPath") == 0) {
    throw std::runtime_error("Ocr.create options.models.classifierModelPath is required.");
  }
  models.classifier_model_path = std::any_cast<std::string>(rawModels.at("classifierModelPath"));
  if (rawModels.count("dictionaryPath") == 0) {
    throw std::runtime_error("Ocr.create options.models.dictionaryPath is required.");
  }
  models.dictionary_path = std::any_cast<std::string>(rawModels.at("dictionaryPath"));
  return options;
}