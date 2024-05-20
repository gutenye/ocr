#include <android/log.h>
#include <jni.h>
#include <jsi/jsi.h>
#include <iostream>
#include "convert-jsi.h"
#include "convert-std.h"
#include "native-ocr.h"

#define LOG_TAG "RNOcrModule"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace facebook::jsi;

std::unique_ptr<NativeOcr> _ocr;
std::string _assetDir;
std::string _debugOutputDir;

void install(Runtime &runtime) {
  auto create = Function::createFromHostFunction(
      runtime, PropNameID::forAscii(runtime, "create"), 1,
      [](Runtime &runtime, const Value &thisValue, const Value *arguments, size_t count) -> Value {
        if (count != 1 || !arguments[0].isObject()) {
          throw JSError(runtime, "Ocr.create: Expected a single options argument");
        }
        Object obj = arguments[0].asObject(runtime);
        auto options = convertJsiObject(runtime, obj);
        _ocr = std::make_unique<NativeOcr>(options, _assetDir, _debugOutputDir);
        return Value::undefined();
      });
  runtime.global().setProperty(runtime, "create", std::move(create));

  auto detect = Function::createFromHostFunction(
      runtime, PropNameID::forAscii(runtime, "detect"), 1,
      [](Runtime &runtime, const Value &thisValue, const Value *arguments, size_t count) -> Value {
        if (count != 1 || !arguments[0].isString()) {
          throw JSError(runtime, "ocr.detect: Expected a single imagePath argument");
        }
        auto imagePath = arguments[0].asString(runtime).utf8(runtime);
        auto lines = _ocr->detect(imagePath);
        return convertStdVector(runtime, lines);
      });
  runtime.global().setProperty(runtime, "detect", std::move(detect));
}

extern "C" JNIEXPORT void JNICALL Java_com_ocr_RNOcrModule_nativeInstall(JNIEnv *env, jobject thiz, jlong jsi,
                                                                         jstring assetDir, jstring debugOutputDir) {
  auto runtime = reinterpret_cast<Runtime *>(jsi);
  install(*runtime);

  const char *cAssetDir = env->GetStringUTFChars(assetDir, nullptr);
  const char *cDebugOuputDir = env->GetStringUTFChars(debugOutputDir, nullptr);
  _assetDir = std::string(cAssetDir);
  _debugOutputDir = std::string(cDebugOuputDir);
  env->ReleaseStringUTFChars(assetDir, cAssetDir);
  env->ReleaseStringUTFChars(debugOutputDir, cDebugOuputDir);

  // JavaVM *java_vm;
  // jobject java_object;
  // env->GetJavaVM(&java_vm);
  // java_object = env->NewGlobalRef(thiz);
}
