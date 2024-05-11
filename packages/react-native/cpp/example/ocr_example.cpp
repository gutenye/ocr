#include <format>
#include <iostream>
#include "../native-ocr.h"

int main() {
  printf("Start\n");

  std::string asset_dir = "../assets";
  auto det_model_file = asset_dir + "/ch_PP-OCRv4_det_infer.onnx";
  auto rec_model_file = asset_dir + "/ch_PP-OCRv4_rec_infer.onnx";
  auto cls_model_file = asset_dir + "/ch_ppocr_mobile_v2";
  auto image_path = asset_dir + "/cn-01.jpg";
  auto output_img_path = asset_dir + "/out.jpg";
  auto dict_path = asset_dir + "/ppocr_keys_v1.txt";
  auto config_path = asset_dir + "/config.txt";

  NativeOcr *pipe_ =
      new NativeOcr(det_model_file, cls_model_file, rec_model_file, "LITE_POWER_HIGH", 1, config_path, dict_path);
  std::vector<std::string> res_txt;
  pipe_->Process(image_path, output_img_path, res_txt);

  std::ostringstream result;
  for (int i = 0; i < res_txt.size() / 2; i++) {
    auto text = res_txt[2 * i];
    auto score = res_txt[2 * i + 1];
    // std::cout << score << "\t" << text << std::endl;
    //   result << i << "\t" << res_txt[2*i] << "\t" << res_txt[2*i + 1] <<
    //   "\t";
    result << res_txt[2 * i] << "\n";
  }
  // std::cout << result.str().c_str() << std::endl;

  return 0;
}