package com.ocr

import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactMethod
import java.io.File
import java.io.FileOutputStream

const val BUNDLE_DIR = "guten-ocr.bundle"
const val OUTPUT_DIR = "guten-ocr.outputs"

class RNOcrModule internal constructor(private val context: ReactApplicationContext) :
    RNOcrSpec(context) {
  companion object {
    const val NAME = "RNOcr"

    init {
      System.loadLibrary("guten-ocr")
    }
  }

  external fun nativeInstall(jsiPtr: Long, assetDir: String, debugOuputDir: String)

  override fun getName(): String {
    return NAME
  }

  override fun initialize() {
    copyAssetToCacheDir()
    install()
  }

  @ReactMethod(isBlockingSynchronousMethod = true)
  override fun install() {
    try {
      val jsContext = getReactApplicationContext().getJavaScriptContextHolder()
      // val jsCallInvokerHolder = context.catalystInstance.jsCallInvokerHolder as
      // CallInvokerHolderImpl

      if (jsContext === null || jsContext.get() == 0L) {
        println("GutenOCR: JSI Runtime is not available in legacy chrome console")
        return
      }

      val assetDir = "${context.cacheDir}/${BUNDLE_DIR}"
      val debugOuputDir = "${context.cacheDir}/${OUTPUT_DIR}"
      nativeInstall(jsContext.get(), assetDir, debugOuputDir)
    } catch (exception: Exception) {
      println("GutenOCR: $exception")
    }
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
