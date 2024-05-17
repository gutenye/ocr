#include "ConvertStd.h"

// std::string -> NSString
NSString *convertStdString(const std::string &stdString) { return [NSString stringWithUTF8String:stdString.c_str()]; }

// std::vector -> NSArray
NSArray<NSString *> *convertStdVector(const std::vector<std::string> &stdVector) {
  NSMutableArray<NSString *> *nsArray = [NSMutableArray arrayWithCapacity:stdVector.size()];
  for (const std::string &stdValue : stdVector) {
    [nsArray addObject:[NSString stringWithUTF8String:stdValue.c_str()]];
  }
  return nsArray;
}
