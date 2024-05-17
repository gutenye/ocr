#include <string>
#include <vector>
#include <Foundation/Foundation.h>

NSString *convertStdString(const std::string &stdString);
NSArray<NSString *> *convertStdVector(const std::vector<std::string> &stdVector);