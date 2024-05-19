package com.ocr

import com.facebook.react.bridge.Promise
import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactContextBaseJavaModule

abstract class RNOcrSpec internal constructor(context: ReactApplicationContext) :
    ReactContextBaseJavaModule(context) {

  // abstract fun create(options: ReadableMap, promise: Promise)

  abstract fun detect(imagePath: String, promise: Promise)

  abstract fun install()
}
