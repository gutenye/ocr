package com.ocr

import com.facebook.react.bridge.Promise
import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactMethod

class RNOcrModule internal constructor(context: ReactApplicationContext) : RNOcrSpec(context) {

  private var ocr: NativeOcr? = null

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
      promise.resolve()
    } catch (e: Exception) {
      promise.reject("JNI_EXCEPTION", "Error initializing NativeOCR: ${e.message}")
    }
  }

  @ReactMethod
  fun detect(imagePath: String, promise: Promise) {
    try {
      val result = nativeDetect(imagePath)
      promise.resolve(result)
    } catch (e: Exception) {
      promise.reject("JNI_EXCEPTION", "Error detecting text: ${e.message}")
    }
  }
}
