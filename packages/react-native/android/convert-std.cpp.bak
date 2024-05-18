#include "convert-std.h"

// Convert std::vector<std::string> to Java List<String>
jobject convertStdVector(const std::vector<std::string> &stdVector) {
  jclass arrayListClass = env->FindClass("java/util/ArrayList");
  jobject arrayList = env->NewObject(arrayListClass, env->GetMethodID(arrayListClass, "<init>", "()V"));
  jmethodID arrayListAdd = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

  for (const auto &line : stdVector) {
    jstring jLine = env->NewStringUTF(line.c_str());
    env->CallBooleanMethod(arrayList, arrayListAdd, jLine);
    env->DeleteLocalRef(jLine);
  }
  return arrayList;
}