import OcrModule from './OcrModule'

export class Ocr {
  static async create(): Promise<Ocr> {
    return new Ocr()
  }

  detect(imagePath: string, options: { isDebug?: boolean } = {}): Promise<string> {
    return OcrModule.detect(imagePath, options)
  }
}
