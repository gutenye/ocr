#include <Foundation/Foundation.h>
#include <string>
#include <vector>
#include "shared.h"

NSString *convertStdString(const std::string &stdString);

NSArray<NSString *> *convertStdVector(const std::vector<std::string> &stdVector);

NSArray *convertStdTextLine(const std::vector<TextLine> &textLines);