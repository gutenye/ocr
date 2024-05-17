#include <any>
#include <filesystem>
#include <format>
#include <iostream>
#include <string>
#include <unordered_map>
#include "../native-ocr.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      throw std::runtime_error("Missing image path. Usage ocr_example <image_path>");
    }
    std::string image_path = argv[1];

    auto source_dir = fs::path(__FILE__).parent_path();
    std::string asset_dir = (source_dir / "assets");
    std::string output_dir = (source_dir / "output");
    std::unordered_map<std::string, std::any> rawOptions {
        {"models",
         std::unordered_map<std::string, std::any> {
             {"detectionModelPath", asset_dir + "/ch_PP-OCRv4_det_infer.onnx"},
             {"recognitionModelPath", asset_dir + "/ch_PP-OCRv4_rec_infer.onnx"},
             // {"classiferModelPath", asset_dir + "/ch_ppocr_mobile_v2"},
             {"dictionaryPath", asset_dir + "/ppocr_keys_v1.txt"},
         }},
        {"isDebug", true},
        {"outputDir", output_dir},
        // {"recognitionImageMaxSize", -1.0},
        {"recognitionImageMaxSize", 800.0},
    };

    fs::remove_all(output_dir);
    fs::create_directory(output_dir);
    NativeOcr* ocr = new NativeOcr(rawOptions);
    auto lines = ocr->detect(image_path);

    // for (auto line : lines) {
    //   std::cout << line << std::endl;
    // }
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "\nError: " << e.what() << std::endl;
    return 1;
  }
}