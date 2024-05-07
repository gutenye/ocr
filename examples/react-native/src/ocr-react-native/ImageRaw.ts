import { ImageRawBase } from '@gutenye/ocr-common'
import type { ImageRawData, LineImage, SizeOption } from '@gutenye/ocr-common'

export class ImageRaw extends ImageRawBase {
  static async open(path: string): Promise<ImageRaw> {}

  constructor(imageRawData: ImageRawData) {
    super(imageRawData)
  }

  async write(path: string) {}

  async resize(size: SizeOption) {}

  async drawBox(lineImages: LineImage[]) {}

  async #apply(sharp: sharp.Sharp) {}
}
