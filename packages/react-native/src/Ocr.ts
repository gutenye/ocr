import OcrModule from './OcrModule'
import type { OcrOptions } from './types'

export class Ocr {
  // default options is at shared.h
  static async create(options: OcrOptions): Promise<Ocr> {
    await OcrModule.create(options)
    return new Ocr()
  }

  detect(rawImagePath: string): Promise<string[]> {
    const imagePath = rawImagePath.replace('file://', '')
    return OcrModule.detect(imagePath)
  }
}
