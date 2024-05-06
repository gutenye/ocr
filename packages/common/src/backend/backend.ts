import type {
  FileUtils as FileUtilsType,
  ImageRaw as ImageRawType,
  InferenceSession as InferenceSessionType,
} from '#/types'

let FileUtils: FileUtilsType = undefined as unknown as FileUtilsType
let ImageRaw: ImageRawType = undefined as unknown as ImageRawType
let InferenceSession: InferenceSessionType = undefined as unknown as InferenceSessionType

export function registerBackend(backend: {
  FileUtils: FileUtilsType
  ImageRaw: ImageRawType
  InferenceSession: InferenceSessionType
}) {
  FileUtils = backend.FileUtils
  ImageRaw = backend.ImageRaw
  InferenceSession = backend.InferenceSession
}

export { FileUtils, ImageRaw, InferenceSession }
