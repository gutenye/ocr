#ifdef __cplusplus
#include <unordered_map>
#include "shared.h"
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

RawOptions convertNSDictionary(NSDictionary *nsDictionary);

NSArray<NSString *> *convertStdVector(const std::vector<std::string> &stdVector);