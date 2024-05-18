package com.ocr

import com.facebook.react.bridge.Promise
import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactMethod
import com.facebook.react.bridge.ReadableArray
import com.facebook.react.bridge.ReadableMap

class RNOcrModule internal constructor(context: ReactApplicationContext) : RNOcrSpec(context) {
  companion object {
    const val NAME = "RNOcr"

    init {
      System.loadLibrary("guten-ocr")
    }
  }

  external fun nativeCreate(options: ReadableMap)
  external fun nativeDetect(imagePath: String): ReadableArray

  override fun getName(): String {
    return NAME
  }

  @ReactMethod
  override fun create(options: ReadableMap, promise: Promise) {
    try {
      nativeCreate(options)
      promise.resolve(null)
    } catch (e: Exception) {
      promise.reject("JNI_EXCEPTION", "Error initializing NativeOCR: ${e.message}")
    }
  }

  @ReactMethod
  override fun detect(imagePath: String, promise: Promise) {
    try {
      val result = nativeDetect(imagePath)
      promise.resolve(result)
    } catch (e: Exception) {
      promise.reject("JNI_EXCEPTION", "Error detecting text: ${e.message}")
    }
  }
}
