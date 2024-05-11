#import "RNOcr.h"
#import <Foundation/Foundation.h>
#import <onnxruntime_cxx_api.h>
#import <iostream>
#import <opencv2/opencv.hpp>
#import <string>
#import <vector>
#import "native-ocr.h"

@interface RNOcr () {
  std::unique_ptr<NativeOcr> _ocr;
}
@end

@implementation RNOcr
RCT_EXPORT_MODULE()

RCT_EXPORT_METHOD(create : (RCTPromiseResolveBlock)resolve rejecter : (RCTPromiseRejectBlock)reject) {
  NSString *rawBundleDir = [[NSBundle mainBundle] bundlePath];
  auto bundleDir = convertNSString(rawBundleDir);
  auto detectionModelPath = bundleDir + "/gutenye-ocr-react-native.bundle/ch_PP-OCRv4_det_infer.onnx";
  auto recognitionModelPath = bundleDir + "/gutenye-ocr-react-native.bundle/ch_PP-OCRv4_rec_infer.onnx";
  auto classifierModelPath = bundleDir + "/gutenye-ocr-react-native.bundle/ch_ppocr_mobile_v2.0_cls_infer.onnx";
  auto dictionaryPath = bundleDir + "/gutenye-ocr-react-native.bundle/ppocr_keys_v1.txt";
  auto configPath = bundleDir + "/gutenye-ocr-react-native.bundle/config.txt";

  _ocr = std::make_unique<NativeOcr>(detectionModelPath, classifierModelPath, recognitionModelPath, configPath,
                                     dictionaryPath);
  long ref = (long)CFBridgingRetain(self);
  resolve(@(ref));
}

RCT_EXPORT_METHOD(detect
                  : (NSString *)rawImagePath options
                  : (NSDictionary *)rawOptions resolver
                  : (RCTPromiseResolveBlock)resolve rejecter
                  : (RCTPromiseRejectBlock)reject) {
  auto imagePath = convertNSString(rawImagePath);
  auto options = convertNSDictionary(rawOptions);
  auto lines = _ocr->Process(imagePath);
  NSArray<NSString *> *finalLines = convertStdVector(lines);
  resolve(finalLines);
}

// Don't compile this code when we build for the old architecture.
#ifdef RCT_NEW_ARCH_ENABLED
- (std::shared_ptr<facebook::react::TurboModule>)getTurboModule:
    (const facebook::react::ObjCTurboModule::InitParams &)params {
  return std::make_shared<facebook::react::NativeOcrSpecJSI>(params);
}
#endif

@end

// convert an NSString to a std::string
std::string convertNSString(NSString *nsString) {
  if (nsString == nil) {
    return std::string();
  }
  return [nsString UTF8String];
}

// convert an NSDictoinary to a std::unordered_map
std::unordered_map<std::string, MapValue> convertNSDictionary(NSDictionary *nsDictionary) {
  std::unordered_map<std::string, MapValue> stdMap{};
  if (nsDictionary == nil) {
    return stdMap;
  }

  for (NSString *rawKey in nsDictionary) {
    std::string key = [rawKey UTF8String];
    id rawValue = [nsDictionary objectForKey:rawKey];
    if ([rawValue isKindOfClass:[NSString class]]) {
      std::string value = [((NSString *)rawValue) UTF8String];
      stdMap[key] = value;
    } else if ([rawValue isKindOfClass:[NSNumber class]]) {
      // Use NSNumber to identify if it's a bool
      if (strcmp([rawValue objCType], @encode(bool)) == 0) {
        bool value = [((NSNumber *)rawValue) boolValue];
        stdMap[key] = value;
      } else {
        double value = [((NSNumber *)rawValue) doubleValue];
        stdMap[key] = value;
      }
    }
  }
  return stdMap;
}

// convert an std::vector to an NSArray
NSArray<NSString *> *convertStdVector(const std::vector<std::string> &stdVector) {
  NSMutableArray<NSString *> *nsArray = [NSMutableArray arrayWithCapacity:stdVector.size()];
  for (const std::string &stdValue : stdVector) {
    [nsArray addObject:[NSString stringWithUTF8String:stdValue.c_str()]];
  }
  return nsArray;
}
