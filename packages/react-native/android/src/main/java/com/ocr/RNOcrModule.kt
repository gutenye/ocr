package com.ocr

import com.facebook.react.bridge.Promise
import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactMethod
import com.facebook.react.bridge.ReadableArray
import com.facebook.react.bridge.ReadableMap
import java.io.File
import java.io.FileOutputStream

const val BUNDLE_DIR = "guten-ocr.bundle"

class RNOcrModule internal constructor(private val context: ReactApplicationContext) :
    RNOcrSpec(context) {
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
      File("${context.cacheDir}/${BUNDLE_DIR}/ppocr_keys_v1.txt").also { println(it.exists()) }
      println(options)
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

  override fun initialize() {
    copyAssetToCacheDir()
  }

  private fun copyAssetToCacheDir() {
    val assetManager = context.assets
    val fileNames = assetManager.list(BUNDLE_DIR) ?: arrayOf()
    for (fileName in fileNames) {
      val srcFile = "$BUNDLE_DIR/$fileName"
      val destDir = File("${context.cacheDir}/$BUNDLE_DIR")
      destDir.mkdirs()
      val destFile = File("$destDir/$fileName")
      if (destFile.exists()) {
        continue
      }
      assetManager.open(srcFile).use { inputStream ->
        FileOutputStream(destFile).use { outputStream -> inputStream.copyTo(outputStream) }
      }
    }
  }
}
