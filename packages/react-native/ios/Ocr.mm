#import "Ocr.h"
#import "pipeline.h"
#import <iostream>
#import <onnxruntime_cxx_api.h>
#import <opencv2/opencv.hpp>
#import <paddle_api.h>
#import <paddle_use_kernels.h>
#import <paddle_use_ops.h>
#import <string>
#import <vector>

void onnx_run(const std::string &model_path);

@implementation Ocr
RCT_EXPORT_MODULE()

RCT_EXPORT_METHOD(ocr
                  : (NSString *)img_path1 resolve
                  : (RCTPromiseResolveBlock)resolve reject
                  : (RCTPromiseRejectBlock)reject) {
  NSString *path = [[NSBundle mainBundle] bundlePath];
  std::string paddle_dir = std::string([path UTF8String]);
  std::string det_model_file =
      paddle_dir + "/resources.bundle/ch_ppocr_mobile_v2.0_det_slim_opt.nb";
  std::string rec_model_file =
      paddle_dir + "/resources.bundle/ch_ppocr_mobile_v2.0_rec_slim_opt.nb";
  std::string cls_model_file =
      paddle_dir + "/resources.bundle/ch_ppocr_mobile_v2.0_cls_slim_opt.nb";
  // std::string det_model_file = paddle_dir +
  // "/resources.bundle/ch_PP-OCRv4_det_infer.onnx";
  std::string rec_model_file2 =
      paddle_dir + "/resources.bundle/ch_PP-OCRv4_rec_infer.onnx";
  // std::string cls_model_file = paddle_dir +
  // "/resources.bundle/ch_ppocr_mobile_v2.0_cls_infer.onnx";
  std::string img_path = paddle_dir + "/resources.bundle/cn-01.jpg";
  std::string output_img_path = paddle_dir + "/test_result.jpg";
  std::string dict_path = paddle_dir + "/resources.bundle/ppocr_keys_v1.txt";
  std::string config_path = paddle_dir + "/resources.bundle/config.txt";

  onnx_run(rec_model_file2);

  cv::Mat srcimg = cv::imread(img_path);
  Pipeline *pipe_ = new Pipeline(det_model_file, cls_model_file, rec_model_file,
                                 "LITE_POWER_HIGH", 1, config_path, dict_path);
  std::vector<std::string> res_txt;
  cv::Mat img_vis = pipe_->Process(srcimg, output_img_path, res_txt);

  std::ostringstream result;
  for (int i = 0; i < res_txt.size() / 2; i++) {
    //   result << i << "\t" << res_txt[2*i] << "\t" << res_txt[2*i + 1] <<
    //   "\t";
    result << res_txt[2 * i] << "\n";
  }
  NSString *text = [NSString stringWithUTF8String:result.str().c_str()];
  NSLog(@"%@", text);

  resolve(text);
}

// Don't compile this code when we build for the old architecture.
#ifdef RCT_NEW_ARCH_ENABLED
- (std::shared_ptr<facebook::react::TurboModule>)getTurboModule:
    (const facebook::react::ObjCTurboModule::InitParams &)params {
  return std::make_shared<facebook::react::NativeOcrSpecJSI>(params);
}
#endif

@end
