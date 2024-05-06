import { ImageRawBase } from '@gutenye/ocr-common'
import type { ImageRawData, LineImage, SizeOption } from '@gutenye/ocr-common'
import invariant from 'tiny-invariant'

export class ImageRaw extends ImageRawBase {
  data: Uint8ClampedArray
  #imageData: ImageData
  #canvas: HTMLCanvasElement

  static async open(url: string): Promise<ImageRaw> {
    const image = await imageFromUrl(url)
    const canvas = document.createElement('canvas')
    canvasDrawImage(canvas, image, image.naturalWidth, image.naturalHeight)
    const imageData = canvasGetImageData(canvas)
    return new ImageRaw({
      data: imageData.data,
      width: imageData.width,
      height: imageData.height,
    })
  }

  constructor({ data, width, height }: ImageRawData) {
    const newData = Uint8ClampedArray.from(data)
    super({
      data: newData,
      width,
      height,
    })
    const canvas = document.createElement('canvas')
    const imageData = new ImageData(newData, width, height)
    canvasPutImageData(canvas, imageData)
    this.#canvas = canvas
    this.#imageData = imageData
    this.data = newData // this.data is undefined without this line
  }

  async write(path: string) {
    document.body.append(this.#canvas)
  }

  async resize({ width, height }: SizeOption) {
    invariant(width !== undefined || height !== undefined, 'both width and height are undefined')
    const newWidth = width || Math.round((this.width / this.height) * height!)
    const newHeight = height || Math.round((this.height / this.width) * width!)
    const newCanvas = document.createElement('canvas')
    canvasDrawImage(newCanvas, this.#canvas, newWidth, newHeight)
    const newImageData = canvasGetImageData(newCanvas)
    return this.#apply(newImageData)
  }

  async drawBox(lineImages: LineImage[]) {
    this.#ctx.strokeStyle = 'red'
    for (const lineImage of lineImages) {
      const [first, ...rests] = lineImage.box
      this.#ctx.beginPath()
      this.#ctx.moveTo(first[0], first[1])
      for (const rest of rests) {
        this.#ctx.lineTo(rest[0], rest[1])
      }
      this.#ctx.closePath()
      this.#ctx.stroke()
    }
    return this
  }

  get #ctx() {
    return this.#canvas.getContext('2d')!
  }

  #apply(imageData: ImageData) {
    canvasPutImageData(this.#canvas, imageData)
    this.#imageData = imageData
    this.data = imageData.data
    this.width = imageData.width
    this.height = imageData.height
    return this
  }

  #putImageData() {
    this.#canvas.width = this.width
    this.#canvas.height = this.height
    this.#ctx.putImageData(this.#imageData, 0, 0)
    return this
  }

  #drawImage(image: CanvasImageSource, width?: number, height?: number) {
    canvasDrawImage(this.#canvas, image, width, height)
    return this
  }
}

function canvasDrawImage(canvas: HTMLCanvasElement, image: CanvasImageSource, width?: number, height?: number) {
  canvas.width = width || image.width
  canvas.height = height || image.height
  const ctx = canvas.getContext('2d')!
  ctx.drawImage(image, 0, 0, canvas.width, canvas.height)
}

function canvasPutImageData(canvas: HTMLCanvasElement, imageData: ImageData, width?: number, height?: number) {
  const ctx = canvas.getContext('2d')!
  canvas.width = width || imageData.width
  canvas.height = height || imageData.height
  ctx.putImageData(imageData, 0, 0)
}

function canvasGetImageData(canvas: HTMLCanvasElement) {
  return canvas.getContext('2d')!.getImageData(0, 0, canvas.width, canvas.height)
}

async function imageFromUrl(url: string) {
  const image = new Image()
  image.src = url
  await image.decode()
  return image
}
