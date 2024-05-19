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

using namespace facebook::jsi;

std::unique_ptr<NativeOcr> _ocr;
std::string _assetDir;
std::string _outputDir;

void install(Runtime &runtime) {
  auto create = Function::createFromHostFunction(
      runtime, PropNameID::forAscii(runtime, "create"), 1,
      [](Runtime &runtime, const Value &thisValue, const Value *arguments, size_t count) -> Value {
        LOGI("%s", _assetDir.c_str());
        return String::createFromAscii(runtime, "Hello from C++!");
      });
  runtime.global().setProperty(runtime, "create", std::move(create));
}

// extern "C" JNIEXPORT void JNICALL Java_com_ocr_RNOcrModule_nativeCreate(JNIEnv *env, jclass type, jobject rawOptions)
// { auto readableMap = adopt_local(static_ref_cast<JMap<jstring, jobject>::javaobject>(rawOptions));

//   auto options = convertReadableMap(env, rawOptions);
//   _ocr = std::make_unique<NativeOcr>(options);
// }

// extern "C" JNIEXPORT jobject JNICALL Java_com_ocr_RNOcrModule_nativeDetect(JNIEnv *env, jclass type,
//                                                                            jstring rawImagePath) {
//   // const char *imagePath = env->GetStringUTFChars(rawImagePath, 0);
//   // auto lines = _ocr->detect(imagePath);
//   // env->ReleaseStringUTFChars(rawImagePath, imagePath);
//   // return convertStdVector(lines);
// }

extern "C" JNIEXPORT void JNICALL Java_com_ocr_RNOcrModule_nativeInstall(JNIEnv *env, jobject thiz, jlong jsi,
                                                                         jstring assetDir, jstring outputDir) {
  auto runtime = reinterpret_cast<Runtime *>(jsi);
  install(*runtime);

  const char *cAssetDir = env->GetStringUTFChars(assetDir, nullptr);
  const char *cOutputDir = env->GetStringUTFChars(outputDir, nullptr);
  _assetDir = std::string(cAssetDir);
  _outputDir = std::string(cOutputDir);
  env->ReleaseStringUTFChars(assetDir, cAssetDir);
  env->ReleaseStringUTFChars(outputDir, cOutputDir);

  // JavaVM *java_vm;
  // jobject java_object;
  // env->GetJavaVM(&java_vm);
  // java_object = env->NewGlobalRef(thiz);
}
