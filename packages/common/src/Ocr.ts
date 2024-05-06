import invariant from 'tiny-invariant'
import { Detection, Recognition } from './models'

export class Ocr {
  static DEFAULT_DETECTION_PATH: string
  static DEFAULT_RECOGINTION_PATH: string
  static DEFAULT_DICTIONARY_PATH: string

  static async create({
    models,
  }: {
    models?: {
      detectionPath: string
      recognitionPath: string
      dictionaryPath: string
    }
  } = {}) {
    const detectionPath = this.DEFAULT_DETECTION_PATH || models?.detectionPath
    invariant(detectionPath, 'detectionPath is required')
    const recognitionPath = this.DEFAULT_RECOGINTION_PATH || models?.recognitionPath
    invariant(recognitionPath, 'recognitionPath is required')
    const dictionaryPath = this.DEFAULT_DICTIONARY_PATH || models?.dictionaryPath
    invariant(dictionaryPath, 'dictionaryPath is required')

    const detection = await Detection.create({
      modelPath: detectionPath,
    })
    const recognition = await Recognition.create({
      modelPath: recognitionPath,
      dictionaryPath,
    })
    return new Ocr({ detection, recognition })
  }

  #detection: Detection
  #recognition: Recognition

  constructor({
    detection,
    recognition,
  }: {
    detection: Detection
    recognition: Recognition
  }) {
    this.#detection = detection
    this.#recognition = recognition
  }

  async detect(image: ImageRaw, options) {
    const lineImages = await this.#detection.run(image, options)
    const texts = await this.#recognition.run(lineImages, options)
    return texts
  }
}
