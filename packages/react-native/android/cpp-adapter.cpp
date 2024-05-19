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
std::string _outputDir;

void processOptions(std::unordered_map<std::string, std::any> &options) {
  if (options.count("outputDir") == 0) {
    options["outputDir"] = _outputDir;
  }
  if (options.count("models") == 0) {
    std::unordered_map<std::string, std::any> models {
        {"detectionModelPath", _assetDir + "/ch_PP-OCRv4_det_infer.onnx"},
        {"recognitionModelPath", _assetDir + "/ch_PP-OCRv4_rec_infer.onnx"},
        {"classifierModelPath", _assetDir + "/ch_ppocr_mobile_v2.0_cls_infer.onnx"},
        {"dictionaryPath", _assetDir + "/ppocr_keys_v1.txt"}};
    options["models"] = models;
  }
}

void install(Runtime &runtime) {
  auto create = Function::createFromHostFunction(
      runtime, PropNameID::forAscii(runtime, "create"), 1,
      [](Runtime &runtime, const Value &thisValue, const Value *arguments, size_t count) -> Value {
        if (count != 1 || !arguments[0].isObject()) {
          throw JSError(runtime, "Ocr.create: Expected a single options argument");
        }
        Object obj = arguments[0].asObject(runtime);
        auto options = convertJsiObject(runtime, obj);
        processOptions(options);
        _ocr = std::make_unique<NativeOcr>(options);
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
