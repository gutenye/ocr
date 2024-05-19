#include <Foundation/Foundation.h>
#include <any>
#include <string>
#include <unordered_map>
#include <vector>

std::string convertNSString(NSString* nsString);

double convertNSNumber(NSNumber* nsNumber);

std::unordered_map<std::string, std::any> convertNSDictionary(NSDictionary* nsDict);

std::vector<std::any> convertNSArray(NSArray* nsArray);

std::any convertNSAny(id objcValue);