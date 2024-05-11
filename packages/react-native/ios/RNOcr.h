#ifdef __cplusplus
#include <unordered_map>
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

using MapValue = std::variant<std::string, double, bool>;
std::unordered_map<std::string, MapValue> convertNSDictionary(NSDictionary *nsDictionary);

NSArray<NSString *> *convertStdVector(const std::vector<std::string> &stdVector);