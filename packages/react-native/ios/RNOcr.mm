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
                  : (NSDictionary *)rawOptions resolver
                  : (RCTPromiseResolveBlock)resolve rejecter
                  : (RCTPromiseRejectBlock)reject) {
  try {
    id bundleDir = [[NSBundle mainBundle] bundlePath];
    id cacheDir = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) firstObject];
    auto assetDir = convertNSString([bundleDir stringByAppendingString:@"/guten-ocr.bundle"]);
    auto debugOutputDir = convertNSString([cacheDir stringByAppendingString:@"/guten-ocr.outputs"]);
    auto options = convertNSDictionary(rawOptions);

    _ocr = std::make_unique<NativeOcr>(options, assetDir, debugOutputDir);

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
