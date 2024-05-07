import { ImageRawBase } from '@gutenye/ocr-common'
import type { LineImage, SizeOption } from '@gutenye/ocr-common'
import { type ImageResult, manipulateAsync } from 'expo-image-manipulator'
import invariant from 'tiny-invariant'

export class ImageRaw extends ImageRawBase {
  static async open(path: string): Promise<ImageRaw> {
    const manipResult = await manipulateAsync(path, [], { base64: true })
    invariant(manipResult.base64, 'ImageRaw.open returns empty base64')
    const data = base64ToUint8Array(manipResult.base64)
    return new ImageRaw({
      data,
      width: manipResult.width,
      height: manipResult.height,
    })
  }

  #uri: string

  constructor({ data, width, height }) {
    super({
      data,
      width,
      height,
    })
    const base64 = uint8ArrayToBase64(data)
    this.#uri = toUri(base64)
  }

  async write(path: string) {}

  async resize(size: SizeOption) {
    const manipResult = await manipulateAsync(this.#uri, [{ width: size.width, height: size.height }], { base64: true })
    this.#apply(manipResult)
  }

  async drawBox(lineImages: LineImage[]) {}

  async #apply(manipResult: ImageResult) {
    invariant(manipResult.base64, '#apply has empty base64')
    this.data = base64ToUint8Array(manipResult.base64)
    this.width = manipResult.width
    this.height = manipResult.height
    this.#uri = toUri(manipResult.base64)
  }
}

function base64ToUint8Array(base64: string): Uint8Array {
  return Uint8Array.from(atob(base64), (c) => c.charCodeAt(0))
}

function uint8ArrayToBase64(data: Uint8Array): string {
  return btoa(String.fromCharCode(...data))
}

function toUri(base64: string): string {
  return `data:image/jpeg;base64,${base64}`
}
