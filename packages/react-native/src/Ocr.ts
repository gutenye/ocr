import OcrModule from './OcrModule'

export class Ocr {
  ocr(imagePath: string): Promise<string> {
    return OcrModule.ocr(imagePath)
  }
}
