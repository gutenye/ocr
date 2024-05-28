#include <any>
#include <filesystem>
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
    std::string debug_output_dir = (source_dir / "output");
    std::unordered_map<std::string, std::any> rawOptions {
        {"isDebug", true},
        // {"recognitionImageMaxSize", -1.0},
        // {"recognitionImageMaxSize", 800.0},
    };

    fs::remove_all(debug_output_dir);
    fs::create_directory(debug_output_dir);
    NativeOcr* ocr = new NativeOcr(rawOptions, asset_dir, debug_output_dir);
    auto text_lines = ocr->detect(image_path);

    // for (auto text_line : text_lines) {
    //   std::cout << text_line.score << " " << text_line.text << std::endl;
    // }
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "\nError: " << e.what() << std::endl;
    return 1;
  }
}