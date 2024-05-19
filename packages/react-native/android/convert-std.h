
#include <jsi/jsi.h>
#include <string>
#include <vector>

using namespace facebook::jsi;
using namespace std;

String convertStdString(Runtime& runtime, const string& stdString);

Array convertStdVector(Runtime& runtime, const vector<string>& stdVector);