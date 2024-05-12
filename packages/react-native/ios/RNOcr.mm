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

  NSLog(@"options: %@", rawOptions);

  std::cout << " string: " << std::get<std::string>(options["string"]) << std::endl;
  std::cout << " double: " << std::get<double>(options["float1"]) << std::endl;
  std::cout << " number: " << std::get<int>(options["number1"]) << std::endl;
  std::cout << " boolean: " << std::get<bool>(options["boolean1"]) << std::endl;

  // for (const auto &pair : options) {
  //   std::visit(
  //       [](auto &&arg) {
  //         using T = std::decay_t<decltype(arg)>;
  //         if constexpr (std::is_same_v<T, std::string>) {
  //           std::cout << "string: " << pair.first << ": " << arg << std::endl;
  //         } else if constexpr (std::is_same_v<T, double>) {
  //           std::cout << "double: " << pair.first << ": " << arg << std::endl;
  //         } else if constexpr (std::is_same_v<T, bool>) {
  //           std::cout << "boolean: " << pair.first << ": " << arg << std::endl;
  //         }
  //       },
  //       pair.second);
  // }

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
        id rawValueString = [rawValue stringValue];
        NSRange range = [rawValueString rangeOfString:@"."];
        NSRange exponentRangeE = [rawValueString rangeOfString:@"e" options:NSCaseInsensitiveSearch];
        if (range.location != NSNotFound || exponentRangeE.location != NSNotFound) {
          stdMap[key] = [rawValue doubleValue];
        } else {
          stdMap[key] = [rawValue intValue];
        }
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
