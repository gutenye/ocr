#include <format>
#include <iostream>
#include "../native-ocr.h"

int main() {
  std::string asset_dir = "../assets";
  RawOptions rawOptions {
      {"isDebug", true},
      {"detectionModelPath", asset_dir + "/ch_PP-OCRv4_det_infer.onnx"},
      {"recognitionModelPath", asset_dir + "/ch_PP-OCRv4_rec_infer.onnx"},
      {"classiferModelPath", asset_dir + "/ch_ppocr_mobile_v2"},
      {"dictionaryPath", asset_dir + "/ppocr_keys_v1.txt"},
  };
  auto image_path = asset_dir + "/cn-01.jpg";

  NativeOcr *pipe_ = new NativeOcr(rawOptions);
  std::vector<std::string> res_txt;
  auto lines = pipe_->Process(image_path);

  for (auto line : lines) {
    std::cout << line << std::endl;
  }

  return 0;
}