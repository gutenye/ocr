import Ocr, { registerBackend } from '@gutenye/ocr-common'
import { splitIntoLineImages } from '@gutenye/ocr-common/splitIntoLineImages'
import { InferenceSession } from 'onnxruntime-web'
import { FileUtils } from './FileUtils'
import { ImageRaw } from './ImageRaw'

registerBackend({ FileUtils, ImageRaw, InferenceSession, splitIntoLineImages })

export * from '@gutenye/ocr-common'
export default Ocr
