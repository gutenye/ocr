export interface OcrOptions {
  isDebug: boolean
  recognitionImageMaxSize: number
  detectionThreshold: number
  detectionBoxThreshold: number
  detectionUnclipRatiop: number
  detectionUseDilate: boolean
  detectionUsePolygonScore: boolean
  detectionUseDirectionClassify: boolean
}
