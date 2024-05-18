#include <jni.h>
// #include "convert-std.h"
#include "native-ocr.h"

std::unique_ptr<NativeOcr> _ocr;

extern "C" JNIEXPORT void JNICALL Java_com_ocr_RNOcrModule_nativeCreate(JNIEnv *env, jclass type, jobject options) {
  _ocr = std::make_unique<NativeOcr>();
}

extern "C" JNIEXPORT jobject JNICALL Java_com_ocr_RNOcrModule_nativeDetect(JNIEnv *env, jclass type,
                                                                           jstring rawImagePath) {
  // const char *imagePath = env->GetStringUTFChars(rawImagePath, 0);
  // auto lines = _ocr->detect(imagePath);
  // env->ReleaseStringUTFChars(rawImagePath, imagePath);
  // return convertStdVector(lines);
}
