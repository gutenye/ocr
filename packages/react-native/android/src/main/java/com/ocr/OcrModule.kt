package com.ocr

import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactMethod
import com.facebook.react.bridge.Promise

class OcrModule internal constructor(context: ReactApplicationContext) :
  OcrSpec(context) {

  override fun getName(): String {
    return NAME
  }

  init {
    System.loadLibrary("react-native-ocr")
  }

  external fun nativeMultiply(a: Double, b: Double): Double

  // Example method
  // See https://reactnative.dev/docs/native-modules-android
  @ReactMethod
  override fun multiply(a: Double, b: Double, promise: Promise) {
    promise.resolve(nativeMultiply(a, b))
  }

  companion object {
    const val NAME = "RNOcr"
  }
}