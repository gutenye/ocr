import type { ImageRawData } from '#common/types'

export class ImageRawBase {
  data: ImageRawData['data']
  width: ImageRawData['width']
  height: ImageRawData['height']

  constructor({ data, width, height }: ImageRawData) {
    this.data = data
    this.width = width
    this.height = height
  }

  getImageRawData(): ImageRawData {
    return { data: this.data, width: this.width, height: this.height }
  }
}
