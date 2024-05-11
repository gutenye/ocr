#import "RNOcr.h"
#import <onnxruntime_cxx_api.h>
#import <iostream>
#import <opencv2/opencv.hpp>
#import <string>
#import <vector>
#import "native-ocr.h"

@implementation RNOcr
RCT_EXPORT_MODULE()

RCT_EXPORT_METHOD(create : (RCTPromiseResolveBlock)resolve rejecter : (RCTPromiseRejectBlock)reject) {
  RNOcr *instance = [RNOcr new];

  NSString *path = [[NSBundle mainBundle] bundlePath];
  std::string paddle_dir = std::string([path UTF8String]);
  std::string det_model_file = paddle_dir + "/gutenye-ocr-react-native.bundle/ch_PP-OCRv4_det_infer.onnx";
  std::string rec_model_file = paddle_dir + "/gutenye-ocr-react-native.bundle/ch_PP-OCRv4_rec_infer.onnx";
  std::string cls_model_file = paddle_dir + "/gutenye-ocr-react-native.bundle/ch_ppocr_mobile_v2.0_cls_infer.onnx";
  std::string dict_path = paddle_dir + "/gutenye-ocr-react-native.bundle/ppocr_keys_v1.txt";
  std::string config_path = paddle_dir + "/gutenye-ocr-react-native.bundle/config.txt";

  std::string img_path = paddle_dir + "/gutenye-ocr-react-native.bundle/cn-01.jpg";
  // std::string output_img_path = paddle_dir + "/test_result.jpg";

  instance.ocr =
      new NativeOcr(det_model_file, cls_model_file, rec_model_file, "LITE_POWER_HIGH", 1, config_path, dict_path);

  resolve([self objectRefForInstance:instance]);
}

RCT_EXPORT_METHOD(detect
                  : (NSString *)image_path options
                  : (NSDictionary *)options

                      resolve
                  : (RCTPromiseResolveBlock)resolve reject
                  : (RCTPromiseRejectBlock)reject) {
  RNOcr *instance = (RNOcr *)(void *)[objectRef integerValue];

  std::vector<std::string> res_txt;
  instance.ocr->Process(img_path, output_img_path, res_txt);

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

- (NSNumber *)objectRefForInstance:(id)instance {
  return @((NSInteger)instance);
}

// Don't compile this code when we build for the old architecture.
#ifdef RCT_NEW_ARCH_ENABLED
- (std::shared_ptr<facebook::react::TurboModule>)getTurboModule:
    (const facebook::react::ObjCTurboModule::InitParams &)params {
  return std::make_shared<facebook::react::NativeOcrSpecJSI>(params);
}
#endif

@end
