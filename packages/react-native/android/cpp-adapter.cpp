#include <jni.h>
#include <jsi/jsi.h>
#include "convert-j.h"
// #include "convert-std.h"
#include <android/log.h>
#include <iostream>
#include "native-ocr.h"

#define LOG_TAG "RNOcrModule"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

std::unique_ptr<NativeOcr> _ocr;

using namespace facebook::jsi;

void install(Runtime &runtime);

extern "C" JNIEXPORT void JNICALL Java_com_ocr_RNOcrModule_nativeCreate(JNIEnv *env, jclass type, jobject rawOptions) {
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

extern "C" JNIEXPORT void JNICALL Java_com_ocr_RNOcrModule_nativeInstall(JNIEnv *env, jobject thiz, jlong jsi) {
  auto runtime = reinterpret_cast<Runtime *>(jsi);
  install(*runtime);
  // JavaVM *java_vm;
  // jobject java_object;
  // env->GetJavaVM(&java_vm);
  // java_object = env->NewGlobalRef(thiz);
}

void install(Runtime &runtime) {
  auto helloWorld = Function::createFromHostFunction(
      runtime, PropNameID::forAscii(runtime, "helloWorld"), 0,
      [](Runtime &runtime, const Value &thisValue, const Value *arguments, size_t count) -> Value {
        return String::createFromAscii(runtime, "Hello from C++!");
      });

  runtime.global().setProperty(runtime, "helloWorld", std::move(helloWorld));
}
