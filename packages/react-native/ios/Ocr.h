#ifdef __cplusplus
#endif

#ifdef RCT_NEW_ARCH_ENABLED
#import "RNOcrSpec.h"

@interface Ocr : NSObject <NativeOcrSpec>
#else
#import <React/RCTBridgeModule.h>

@interface Ocr : NSObject <RCTBridgeModule>
#endif

@end
