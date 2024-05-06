import Ocr, { registerBackend } from '@gutenye/ocr-common'
import { InferenceSession } from 'onnxruntime-web'
import { FileUtils } from './FileUtils'
import { ImageRaw } from './ImageRaw'

registerBackend({ FileUtils, ImageRaw, InferenceSession })

export * from '@gutenye/ocr-common'
export default Ocr
