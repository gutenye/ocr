#include <jni.h>
#include "react-native-ocr.h"

extern "C"
JNIEXPORT jdouble JNICALL
Java_com_ocr_OcrModule_nativeMultiply(JNIEnv *env, jclass type, jdouble a, jdouble b) {
    return ocr::multiply(a, b);
}
