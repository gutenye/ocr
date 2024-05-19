#include "convert-std.h"

using namespace facebook::jsi;
using namespace std;

String convertStdString(Runtime& runtime, const string& stdString) {return String::createFromUtf8(runtime, stdString);}

Array convertStdVector(Runtime& runtime, const vector<string>& stdVector) {
  Array jsiArray(runtime, stdVector.size());
  for (size_t i = 0; i < stdVector.size(); ++i) {
    auto jsiString = convertStdString(runtime, stdVector[i]);
    jsiArray.setValueAtIndex(runtime, i, jsiString);
  }
  return jsiArray;
}
