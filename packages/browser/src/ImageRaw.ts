import { ImageRawBase } from '@gutenye/ocr-common'
import type { ImageRawData, LineImage, SizeOption } from '@gutenye/ocr-common'
import invariant from 'tiny-invariant'

export class ImageRaw extends ImageRawBase {
  data: Uint8ClampedArray;
  #imageData: ImageData;
  #canvas: OffscreenCanvas;

  static async open(url: string): Promise<ImageRaw> {
    const imageBitmap = await imageBitmapFromUrl(url);
    const canvas = createCanvas(imageBitmap.width, imageBitmap.height);
    canvasDrawImage(canvas, imageBitmap, imageBitmap.width, imageBitmap.height);
    const imageData = canvasGetImageData(canvas);
    return new ImageRaw({
      data: imageData.data,
      width: imageData.width,
      height: imageData.height,
    });
  }

  constructor({ data, width, height }: ImageRawData) {
    const newData = Uint8ClampedArray.from(data);
    super({
      data: newData,
      width,
      height,
    });

    const canvas = createCanvas(width, height);
    const imageData = new ImageData(newData, width, height);
    canvasPutImageData(canvas, imageData);

    this.#canvas = canvas;
    this.#imageData = imageData;
    this.data = newData; // ensure public field available
  }

  async write(path?: string): Promise<Blob> {
    return await this.#canvas.convertToBlob();
  }

  async resize({ width, height }: SizeOption) {
    invariant(
      width !== undefined || height !== undefined,
      "both width and height are undefined"
    );
    const newWidth = width || Math.round((this.width / this.height) * height!);
    const newHeight = height || Math.round((this.height / this.width) * width!);
    const newCanvas = createCanvas(newWidth, newHeight);
    canvasDrawImage(newCanvas, this.#canvas, newWidth, newHeight);
    const newImageData = canvasGetImageData(newCanvas);
    return this.#apply(newImageData);
  }

  async drawBox(lineImages: LineImage[]) {
    const ctx = this.#ctx;
    ctx.strokeStyle = "red";
    for (const lineImage of lineImages) {
      const [first, ...rests] = lineImage.box;
      ctx.beginPath();
      ctx.moveTo(first[0], first[1]);
      for (const rest of rests) {
        ctx.lineTo(rest[0], rest[1]);
      }
      ctx.closePath();
      ctx.stroke();
    }
    return this;
  }

  get #ctx(): OffscreenCanvasRenderingContext2D {
    const ctx = this.#canvas.getContext("2d");
    if (!ctx) throw new Error("2D context not available on OffscreenCanvas");
    return ctx;
  }

  #apply(imageData: ImageData) {
    canvasPutImageData(this.#canvas, imageData);
    this.#imageData = imageData;
    this.data = imageData.data;
    this.width = imageData.width;
    this.height = imageData.height;
    return this;
  }
}

/* ------------------- Helpers (OffscreenCanvas-only) ------------------- */

function createCanvas(width = 1, height = 1): OffscreenCanvas {
  // Strictly worker/OffscreenCanvas environment
  if (typeof OffscreenCanvas === "undefined") {
    throw new Error("OffscreenCanvas is not available in this environment");
  }
  return new OffscreenCanvas(width, height);
}

function canvasDrawImage(
  canvas: OffscreenCanvas,
  // restrict to worker-friendly sources
  image: ImageBitmap | OffscreenCanvas,
  width?: number,
  height?: number
) {
  const w = width ?? (image as any).width ?? canvas.width;
  const h = height ?? (image as any).height ?? canvas.height;
  canvas.width = w;
  canvas.height = h;
  const ctx = canvas.getContext("2d");
  if (!ctx) throw new Error("2D context not available on OffscreenCanvas");
  ctx.drawImage(image as any, 0, 0, w, h);
}

function canvasPutImageData(
  canvas: OffscreenCanvas,
  imageData: ImageData,
  width?: number,
  height?: number
) {
  const w = width ?? imageData.width;
  const h = height ?? imageData.height;
  canvas.width = w;
  canvas.height = h;
  const ctx = canvas.getContext("2d");
  if (!ctx) throw new Error("2D context not available on OffscreenCanvas");
  ctx.putImageData(imageData, 0, 0);
}

function canvasGetImageData(canvas: OffscreenCanvas) {
  const ctx = canvas.getContext("2d");
  if (!ctx) throw new Error("2D context not available on OffscreenCanvas");
  return ctx.getImageData(0, 0, canvas.width, canvas.height);
}

async function imageBitmapFromUrl(url: string): Promise<ImageBitmap> {
  const res = await fetch(url);
  if (!res.ok) throw new Error(`Failed to fetch image: ${res.status}`);
  const blob = await res.blob();

  if (typeof createImageBitmap === "undefined") {
    throw new Error("createImageBitmap is not available in this environment");
  }
  const bitmap = await createImageBitmap(blob);
  return bitmap;
}
