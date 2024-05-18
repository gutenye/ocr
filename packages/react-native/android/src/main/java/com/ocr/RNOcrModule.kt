package com.ocr

import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactMethod
import com.facebook.react.bridge.Promise

class RNOcrModule internal constructor(context: ReactApplicationContext) :
  RNOcrSpec(context) {

  companion object {
    const val NAME = "RNOcr"
  }

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
}