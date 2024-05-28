import OcrModule from './OcrModule'
import type { OcrOptions, TextLine } from './types'

export class Ocr {
  // default options is at shared.h
  static async create(options: OcrOptions): Promise<Ocr> {
    await OcrModule.create(options)
    return new Ocr()
  }

  detect(rawImagePath: string): Promise<TextLine[]> {
    const imagePath = rawImagePath.replace('file://', '')
    return OcrModule.detect(imagePath)
  }
}
