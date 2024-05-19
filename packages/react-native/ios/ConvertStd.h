#include <Foundation/Foundation.h>
#include <string>
#include <vector>

NSString *convertStdString(const std::string &stdString);

NSArray<NSString *> *convertStdVector(const std::vector<std::string> &stdVector);