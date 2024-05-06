import Ocr, { registerBackend } from '@gutenye/ocr-common'
import useDefaultModels from '@gutenye/ocr-common/use-default-models.node'
import { InferenceSession } from 'onnxruntime-node'
import { FileUtils } from './FileUtils'
import { ImageRaw } from './ImageRaw'

useDefaultModels()
registerBackend({ FileUtils, ImageRaw, InferenceSession })

export * from '@gutenye/ocr-common'
export default Ocr
