#ifdef __cplusplus
#import <unordered_map>
#import "shared.h"
#endif

#import <Foundation/Foundation.h>
#import <memory>
#import "ConvertNS.h"
#import "ConvertStd.h"

#ifdef RCT_NEW_ARCH_ENABLED
#import "RNOcrSpec.h"

@interface RNOcr : NSObject <NativeOcrSpec>
#else
#import <React/RCTBridgeModule.h>

@interface RNOcr : NSObject <RCTBridgeModule>
#endif

@end