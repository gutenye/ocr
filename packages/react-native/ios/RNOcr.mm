#import "RNOcr.h"
#import <Foundation/Foundation.h>
#include <cstring>
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
  auto options = convertNSDictionary(rawOptions);

  id rawBundleDir = [[NSBundle mainBundle] bundlePath];
  auto assetDir = convertNSString(rawBundleDir) + "/guten-ocr.bundle";
  if (!options.count("detectionModelPath")) {
    options["detectionModelPath"] = assetDir + "/ch_PP-OCRv4_det_infer.onnx";
  }
  if (!options.count("recognitionModelPath")) {
    options["recognitionModelPath"] = assetDir + "/ch_PP-OCRv4_rec_infer.onnx";
  }
  if (!options.count("classifierModelPath")) {
    options["classifierModelPath"] = assetDir + "/ch_ppocr_mobile_v2.0_cls_infer.onnx";
  }
  if (!options.count("dictionaryPath")) {
    options["dictionaryPath"] = assetDir + "/ppocr_keys_v1.txt";
  }

  _ocr = std::make_unique<NativeOcr>(options);

  long ref = (long)CFBridgingRetain(self);
  resolve(@(ref));
}

RCT_EXPORT_METHOD(detect
                  : (NSString *)rawImagePath resolver
                  : (RCTPromiseResolveBlock)resolve rejecter
                  : (RCTPromiseRejectBlock)reject) {
  auto imagePath = convertNSString(rawImagePath);
  auto lines = _ocr->process(imagePath);
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
RawOptions convertNSDictionary(NSDictionary *nsDictionary) {
  RawOptions stdMap {};
  if (nsDictionary == nil) {
    return stdMap;
  }
  for (id rawKey in nsDictionary) {
    if (![rawKey isKindOfClass:[NSString class]]) {
      continue;
    }
    std::string key = [rawKey UTF8String];
    id rawValue = [nsDictionary objectForKey:rawKey];
    if ([rawValue isKindOfClass:[NSString class]]) {
      stdMap[key] = std::string([rawValue UTF8String]);
    } else if ([rawValue isKindOfClass:[NSNumber class]]) {
      if (strcmp([rawValue objCType], @encode(char)) == 0) {
        stdMap[key] = (bool)[rawValue boolValue];
      } else {
        stdMap[key] = [rawValue doubleValue];
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
