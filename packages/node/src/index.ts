import Ocr, { registerBackend } from '@gutenye/ocr-common'
import { splitIntoLineImages } from '@gutenye/ocr-common/splitIntoLineImages'
import defaultModels from '@gutenye/ocr-models/node'
import { InferenceSession } from 'onnxruntime-node'
import { FileUtils } from './FileUtils'
import { ImageRaw } from './ImageRaw'

registerBackend({
  FileUtils,
  ImageRaw,
  InferenceSession,
  splitIntoLineImages,
  defaultModels,
})

export * from '@gutenye/ocr-common'
export default Ocr
