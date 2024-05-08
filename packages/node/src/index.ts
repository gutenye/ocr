import Ocr, { registerBackend } from '@gutenye/ocr-common'
import models from '@gutenye/ocr-models/node'
import { InferenceSession } from 'onnxruntime-node'
import { FileUtils } from './FileUtils'
import { ImageRaw } from './ImageRaw'

registerBackend({ FileUtils, ImageRaw, InferenceSession })

Ocr.DEFAULT_DETECTION_PATH = models.detectionPath
Ocr.DEFAULT_RECOGINTION_PATH = models.recognitionPath
Ocr.DEFAULT_DICTIONARY_PATH = models.dictionaryPath

export * from '@gutenye/ocr-common'
export default Ocr
