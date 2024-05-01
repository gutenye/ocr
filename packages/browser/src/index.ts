import Ocr, { registerBackend } from '@gutenye/ocr-common'
import { FileUtils } from './FileUtils'
import { ImageRaw } from './ImageRaw'
import { InferenceSession } from 'onnxruntime-web'

registerBackend({ FileUtils, ImageRaw, InferenceSession })

export * from '@gutenye/ocr-common'
export default Ocr
