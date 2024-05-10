#include "pipeline.h"
#include "run_onnx.h"
#include <iostream>

int main()
{
	std::string asset_dir = "../assets";
	std::string det_model_file = asset_dir +
															 "/ch_PP-OCRv4_det_infer.onnx";
	std::string rec_model_file = asset_dir +
															 "/ch_PP-OCRv4_rec_infer.onnx";
	std::string cls_model_file = asset_dir +
															 "/ch_ppocr_mobile_v2";
	std::string image_path = asset_dir + "/cn-01.jpg";
	std::string output_img_path = asset_dir + "/out.jpg";
	std::string dict_path = asset_dir + "/ppocr_keys_v1.txt";
	std::string config_path = asset_dir + "/config.txt";

	// run_onnx("./assets/ch_PP-OCRv4_rec_infer.onnx");
	// onnx_run(rec_model_file2);

	printf("Running\n");

	cv::Mat srcimg = cv::imread(image_path);
	Pipeline *pipe_ = new Pipeline(det_model_file, cls_model_file, rec_model_file,
																 "LITE_POWER_HIGH", 1, config_path, dict_path);
	std::vector<std::string> res_txt;
	cv::Mat img_vis = pipe_->Process(srcimg, output_img_path, res_txt);

	std::ostringstream result;
	for (int i = 0; i < res_txt.size() / 2; i++)
	{
		//   result << i << "\t" << res_txt[2*i] << "\t" << res_txt[2*i + 1] <<
		//   "\t";
		result << res_txt[2 * i] << "\n";
	}
	std::cout << result.str().c_str() << std::endl;

	return 0;
}