import Ocr, { registerBackend } from '@gutenye/ocr-common'
import useDefaultModels from '@gutenye/ocr-common/use-default-models.node'
import { FileUtils } from './FileUtils'
import { ImageRaw } from './ImageRaw'
import { InferenceSession } from 'onnxruntime-node'

useDefaultModels()
registerBackend({ FileUtils, ImageRaw, InferenceSession })

export * from '@gutenye/ocr-common'
export default Ocr
