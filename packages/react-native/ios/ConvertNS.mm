#include "ConvertNS.h"

// NSString -> std::string
std::string convertNSString(NSString *nsString) { return [nsString UTF8String]; }

// NSNumber -> double
double convertNSNumber(NSNumber *nsNumber) { return [nsNumber doubleValue]; }

// NSDictionary -> std::unordered_map<std::string, std::any>
std::unordered_map<std::string, std::any> convertNSDictionary(NSDictionary *nsDict) {
  std::unordered_map<std::string, std::any> cppMap {};
  for (id key in nsDict) {
    if (![key isKindOfClass:[NSString class]]) {
      id reason = [NSString
          stringWithFormat:@"convertNSDictionary: invalid NSDictionary key type for key '%@', expected 'NSString' type",
                           key];
      @throw [NSException exceptionWithName:@"CustomException" reason:reason userInfo:nil];
    }
    std::string cppKey = convertNSString((NSString *)key);
    std::any cppValue = convertNSAny([nsDict objectForKey:key]);
    cppMap.emplace(cppKey, cppValue);
  }
  return cppMap;
}

// NSArray -> std::vector<std::any>
std::vector<std::any> convertNSArray(NSArray *nsArray) {
  std::vector<std::any> cppVector;
  for (id elem in nsArray) {
    cppVector.push_back(convertNSAny(elem));
  }
  return cppVector;
}

// id -> std::any
std::any convertNSAny(id objcValue) {
  if ([objcValue isKindOfClass:[NSString class]]) {
    return convertNSString((NSString *)objcValue);
  } else if ([objcValue isKindOfClass:[NSNumber class]]) {
    if (strcmp([objcValue objCType], @encode(char)) == 0) {
      return (bool)[objcValue boolValue];
    } else {
      return convertNSNumber((NSNumber *)objcValue);
    }
  } else if ([objcValue isKindOfClass:[NSDictionary class]]) {
    return convertNSDictionary((NSDictionary *)objcValue);
  } else if ([objcValue isKindOfClass:[NSArray class]]) {
    return convertNSArray((NSArray *)objcValue);
  } else {
    // Return as an empty std::any for unsupported types
    return std::any();
  }
}