#import "RNOcr.h"
#import <iostream>
#import "native-ocr.h"

@interface RNOcr () {
  std::unique_ptr<NativeOcr> _ocr;
}
@end

@implementation RNOcr
RCT_EXPORT_MODULE()

RCT_EXPORT_METHOD(create
                  : (NSDictionary *)rawReadonlyOptions resolver
                  : (RCTPromiseResolveBlock)resolve rejecter
                  : (RCTPromiseRejectBlock)reject) {
  try {
    id rawBundleDir = [[NSBundle mainBundle] bundlePath];
    id cacheDir = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) firstObject];

    NSMutableDictionary *rawOptions = [rawReadonlyOptions mutableCopy];
    if (!rawOptions[@"debugOuputDir"]) {
      rawOptions[@"debugOuputDir"] = [cacheDir stringByAppendingString:@"/guten-ocr.outputs"];
    }
    if (!rawOptions[@"models"]) {
      rawOptions[@"models"] = [NSMutableDictionary dictionary];
    }
    NSMutableDictionary *models = rawOptions[@"models"];
    auto assetDir = [rawBundleDir stringByAppendingString:@"/guten-ocr.bundle"];
    if (!models[@"detectionModelPath"]) {
      models[@"detectionModelPath"] = [assetDir stringByAppendingString:@"/ch_PP-OCRv4_det_infer.onnx"];
    }
    if (!models[@"recognitionModelPath"]) {
      models[@"recognitionModelPath"] = [assetDir stringByAppendingString:@"/ch_PP-OCRv4_rec_infer.onnx"];
    }
    if (!models[@"classifierModelPath"]) {
      models[@"classifierModelPath"] = [assetDir stringByAppendingString:@"/ch_ppocr_mobile_v2.0_cls_infer.onnx"];
    }
    if (!models[@"dictionaryPath"]) {
      models[@"dictionaryPath"] = [assetDir stringByAppendingString:@"/ppocr_keys_v1.txt"];
    }

    auto options = convertNSDictionary(rawOptions);
    _ocr = std::make_unique<NativeOcr>(options);

    long ref = (long)CFBridgingRetain(self);
    resolve(@(ref));
  } catch (const std::exception &error) {
    auto message = std::string("Error: ") + error.what();
    reject(@"cpp_exception", convertStdString(message), nil);
  }
}

RCT_EXPORT_METHOD(detect
                  : (NSString *)rawImagePath resolver
                  : (RCTPromiseResolveBlock)resolve rejecter
                  : (RCTPromiseRejectBlock)reject) {
  try {
    auto imagePath = convertNSString(rawImagePath);
    auto lines = _ocr->detect(imagePath);
    resolve(convertStdVector(lines));
  } catch (const std::exception &error) {
    auto message = std::string("Error: ") + error.what();
    reject(@"cpp_exception", convertStdString(message), nil);
  }
}

// Don't compile this code when we build for the old architecture.
#ifdef RCT_NEW_ARCH_ENABLED
- (std::shared_ptr<facebook::react::TurboModule>)getTurboModule:
    (const facebook::react::ObjCTurboModule::InitParams &)params {
  return std::make_shared<facebook::react::NativeOcrSpecJSI>(params);
}
#endif

@end
