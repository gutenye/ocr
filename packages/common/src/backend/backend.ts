import type {
  FileUtils as FileUtilsType,
  ImageRaw as ImageRawType,
  InferenceSession as InferenceSessionType,
  SplitIntoLineImages as SplitIntoLineImagesType,
} from '#/types'

let FileUtils: FileUtilsType = undefined as unknown as FileUtilsType
let ImageRaw: ImageRawType = undefined as unknown as ImageRawType
let InferenceSession: InferenceSessionType = undefined as unknown as InferenceSessionType
let splitIntoLineImages: SplitIntoLineImagesType = undefined as unknown as SplitIntoLineImagesType

export function registerBackend(backend: {
  FileUtils: FileUtilsType
  ImageRaw: ImageRawType
  InferenceSession: InferenceSessionType
  splitIntoLineImages: SplitIntoLineImagesType
}) {
  FileUtils = backend.FileUtils
  ImageRaw = backend.ImageRaw
  InferenceSession = backend.InferenceSession
  splitIntoLineImages = backend.splitIntoLineImages
}

export { FileUtils, ImageRaw, InferenceSession, splitIntoLineImages }
