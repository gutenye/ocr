import OcrModule from './OcrModule'

export class Ocr {
  static async create(): Promise<Ocr> {
    await OcrModule.create()
    return new Ocr()
  }

  detect(imagePath: string, options: { isDebug?: boolean } = {}): Promise<string> {
    return OcrModule.detect(imagePath, options)
  }

  // clean up memory when ocr is not needed.
  destory() {
    return OcrModule.destory()
  }
}
