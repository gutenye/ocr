#include <jni.h>
// #include "convert-std.h"
#include <android/log.h>
#include <iostream>
#include "native-ocr.h"

#define LOG_TAG "RNOcrModule"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

std::unique_ptr<NativeOcr> _ocr;

extern "C" JNIEXPORT void JNICALL Java_com_ocr_RNOcrModule_nativeCreate(JNIEnv *env, jclass type, jobject rawOptions) {
  jclass readableMapClass = env->GetObjectClass(rawOptions);
  // Method ID for `getBoolean` method
  jmethodID getBooleanMethod = env->GetMethodID(readableMapClass, "getBoolean", "(Ljava/lang/String;)Z");
  // Create a Java string holding the name of the key we are interested in
  jstring keyString = env->NewStringUTF("isDebug");
  // Call the `getBoolean` method of `ReadableMap`
  jboolean isDebug = env->CallBooleanMethod(rawOptions, getBooleanMethod, keyString);
  // Print the value of `isDebug`
  LOGI("isDebug: %s", (isDebug ? "true" : "false"));

  // Clean up local references
  env->DeleteLocalRef(keyString);
  env->DeleteLocalRef(readableMapClass);

  // std::unordered_map<std::string, std::any> options {};
  // _ocr = std::make_unique<NativeOcr>(options);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_ocr_RNOcrModule_nativeDetect(JNIEnv *env, jclass type,
                                                                           jstring rawImagePath) {
  // const char *imagePath = env->GetStringUTFChars(rawImagePath, 0);
  // auto lines = _ocr->detect(imagePath);
  // env->ReleaseStringUTFChars(rawImagePath, imagePath);
  // return convertStdVector(lines);
}
