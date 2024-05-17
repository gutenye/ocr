import type { InferenceSession } from 'onnxruntime-common'

export interface OcrOptions {
  models?: {
    detectionPath: string
    recognitionPath: string
    dictionaryPath: string
  }
  isDebug?: boolean
  recognitionImageMaxSize?: number
  detectionThreshold?: number
  detectionBoxThreshold?: number
  detectionUnclipRatiop?: number
  detectionUseDilate?: boolean
  detectionUsePolygonScore?: boolean
  detectionUseDirectionClassify?: boolean
  onnxOptions?: InferenceSession.SessionOptions
}
