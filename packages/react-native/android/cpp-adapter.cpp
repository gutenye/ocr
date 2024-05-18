#include <jni.h>
// #include "react-native-ocr.h"

extern "C" JNIEXPORT jdouble JNICALL Java_com_ocr_RNOcrModule_nativeMultiply(JNIEnv *env, jclass type, jdouble a,
                                                                             jdouble b) {
  return a + 3;
  // return ocr::multiply(a, b);
}
