package com.ocr

import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactContextBaseJavaModule

abstract class RNOcrSpec internal constructor(context: ReactApplicationContext) :
    ReactContextBaseJavaModule(context) {

  abstract fun install()
}
