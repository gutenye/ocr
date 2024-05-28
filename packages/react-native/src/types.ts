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
  useDirectionClassify?: boolean
  onnxOptions?: InferenceSession.SessionOptions
}

export interface TextLine {
  text: string
  score: number
  frame: Frame
}

export interface Frame {
  top: number
  left: number
  width: number
  height: number
}
