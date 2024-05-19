#include <jni.h>
#include "convert-j.h"
// #include "convert-std.h"
#include <android/log.h>
#include <fbjni/fbjni.h>
#include <jsi/jsi.h>
#include <iostream>
#include "native-ocr.h"

#define LOG_TAG "RNOcrModule"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

std::unique_ptr<NativeOcr> _ocr;

using namespace facebook::jni;
using namespace facebook::jsi;

static void nativeCreate(alias_ref<JReadableMap> options) {
  for (auto entry : options) {
    LOGI("key: %s", entry.first.toString());
    // std::string key = entry.first->toString()->toStdString();
    // std::string value = entry.second->toString()->toStdString();
  }

  // auto cplusplusMap = options->toStdUnorderedMap();
  // Further processing with the C++ unordered_map
}

extern "C" JNIEXPORT void JNICALL Java_com_ocr_RNOcrModule_nativeCreate(JNIEnv *env, jclass type, jobject rawOptions) {
  nativeCreate(make_local(options));

  // auto readableMap = adopt_local(static_ref_cast<JMap<jstring, jobject>::javaobject>(rawOptions));

  //   auto options = convertReadableMap(env, rawOptions);
  //   _ocr = std::make_unique<NativeOcr>(options);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_ocr_RNOcrModule_nativeDetect(JNIEnv *env, jclass type,
                                                                           jstring rawImagePath) {
  // const char *imagePath = env->GetStringUTFChars(rawImagePath, 0);
  // auto lines = _ocr->detect(imagePath);
  // env->ReleaseStringUTFChars(rawImagePath, imagePath);
  // return convertStdVector(lines);
}
