import { type InferenceSession as InferenceSessionCommon, Tensor } from 'onnxruntime-common'
import type {
  ImageRaw,
  InferenceSession,
  LineImage,
  ModelBaseConstructorArg,
  ModelBaseOptions,
  ModelData,
  ReshapeOptions,
} from '#common/types'

export class ModelBase {
  options: ModelBaseOptions
  #model: InferenceSession

  constructor({ model, options }: ModelBaseConstructorArg) {
    this.#model = model
    this.options = options
  }

  async runModel({
    modelData,
    onnxOptions = {},
  }: { modelData: ModelData; onnxOptions?: InferenceSessionCommon.RunOptions }) {
    const input = this.#prepareInput(modelData)
    const outputs = await this.#model.run(
      {
        [this.#model.inputNames[0]]: input,
      },
      onnxOptions,
    )
    const output = outputs[this.#model.outputNames[0]]
    return output
  }

  #prepareInput(modelData: ModelData) {
    const input = Float32Array.from(modelData.data)
    return new Tensor('float32', input, [1, 3, modelData.height, modelData.width])
  }

  imageToInput(image: ImageRaw, { mean = [0, 0, 0], std = [1, 1, 1] }: ReshapeOptions): ModelData {
    const R: number[] = []
    const G: number[] = []
    const B: number[] = []
    for (let i = 0; i < image.data.length; i += 4) {
      R.push((image.data[i] / 255 - mean[0]) / std[0])
      G.push((image.data[i + 1] / 255 - mean[1]) / std[1])
      B.push((image.data[i + 2] / 255 - mean[2]) / std[2])
    }
    const newData = [...B, ...G, ...R]
    return {
      data: newData,
      width: image.width,
      height: image.height,
    }
  }

  debugImage(image: ImageRaw | any, path: string) {
    const { debugOutputDir, isDebug } = this.options
    if (!isDebug || !debugOutputDir) {
      return
    }
    image.write(`${debugOutputDir}/${path}`)
  }

  async debugBoxImage(sourceImage: ImageRaw | any, lineImages: LineImage[], path: string) {
    const { debugOutputDir, isDebug } = this.options
    if (!isDebug || !debugOutputDir) {
      return
    }
    const boxImage = await sourceImage.drawBox(lineImages)
    boxImage.write(`${debugOutputDir}/${path}`)
  }
}
