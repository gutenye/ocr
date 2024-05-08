package com.ocr;

import com.facebook.react.bridge.ReactApplicationContext;

abstract class OcrSpec extends NativeOcrSpec {
  OcrSpec(ReactApplicationContext context) {
    super(context);
  }
}
