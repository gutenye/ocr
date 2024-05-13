#include <filesystem>
#include <format>
#include <iostream>
#include "../native-ocr.h"

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      throw std::runtime_error("Missing image path. Usage ocr_example <image_path>");
    }
    std::string image_path = argv[1];

    std::string asset_dir = (std::filesystem::path(__FILE__).parent_path() / "assets");
    RawOptions rawOptions {
        {"isDebug", true},
        // {"recognitionImageMaxSize", -1.0},
        // {"recognitionImageMaxSize", 960.0},
        {"detectionModelPath", asset_dir + "/ch_PP-OCRv4_det_infer.onnx"},
        {"recognitionModelPath", asset_dir + "/ch_PP-OCRv4_rec_infer.onnx"},
        // {"classiferModelPath", asset_dir + "/ch_ppocr_mobile_v2"},
        {"dictionaryPath", asset_dir + "/ppocr_keys_v1.txt"},
    };

    NativeOcr* ocr = new NativeOcr(rawOptions);
    auto lines = ocr->process(image_path);

    // for (auto line : lines) {
    //   std::cout << line << std::endl;
    // }
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "\nError: " << e.what() << std::endl;
    return 1;
  }
}