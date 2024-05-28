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

NSArray *convertStdTextLine(const std::vector<TextLine> &textLines) {
  NSMutableArray *array = [NSMutableArray arrayWithCapacity:textLines.size()];
  for (const TextLine &textLine : textLines) {
    NSString *text = [NSString stringWithUTF8String:textLine.text.c_str()];
    NSDictionary *frameDict = @{
      @"top" : @(textLine.frame.top),
      @"left" : @(textLine.frame.left),
      @"width" : @(textLine.frame.width),
      @"height" : @(textLine.frame.height)
    };
    NSDictionary *textLineDict = @{@"text" : text, @"score" : @(textLine.score), @"frame" : frameDict};
    [array addObject:textLineDict];
  }
  return [array copy];
}
