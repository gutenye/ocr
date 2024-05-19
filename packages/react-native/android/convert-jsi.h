#include <jsi/jsi.h>
#include <any>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using namespace facebook::jsi;
using namespace std;

vector<any> convertJsiArray(Runtime& runtime, const Array& arr);

unordered_map<string, any> convertJsiObject(Runtime& runtime, const Object& obj);

any convertJsiValue(Runtime& runtime, const Value& value);