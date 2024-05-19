#include <jni.h>
#include <any>
#include <string>
#include <unordered_map>
#include <vector>

std::string convertJstring(JNIEnv* env, jstring jvalue);

std::vector<std::any> convertReadableArray(JNIEnv* env, jobject readableArray);
std::unordered_map<std::string, std::any> convertReadableMap(JNIEnv* env, jobject readableMap);