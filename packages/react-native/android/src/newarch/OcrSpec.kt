package com.ocr

import com.facebook.react.bridge.ReactApplicationContext

abstract class OcrSpec internal constructor(context: ReactApplicationContext) :
  NativeOcrSpec(context) {
}