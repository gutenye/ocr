export interface OcrOptions {
  isDebug: boolean
  imageMaxSize: number
  detectionThreshold: number
  detectionBoxThreshold: number
  detectionUnclipRatiop: number
  detectionUseDilate: boolean
  detectionUsePolygonScore: boolean
  detectionUseDirectionClassify: boolean
}
