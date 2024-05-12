#ifdef __cplusplus
#include <unordered_map>
#include <variant>
#endif

#import <memory>

#ifdef RCT_NEW_ARCH_ENABLED
#import "RNOcrSpec.h"

@interface RNOcr : NSObject <NativeOcrSpec>
#else
#import <React/RCTBridgeModule.h>

@interface RNOcr : NSObject <RCTBridgeModule>
#endif

@end

std::string convertNSString(NSString *nsString);

using RawOptions = std::unordered_map<std::string, std::variant<bool, int, double, std::string>>;

RawOptions convertNSDictionary(NSDictionary *nsDictionary);

NSArray<NSString *> *convertStdVector(const std::vector<std::string> &stdVector);