import type { InferenceSession as InferenceSessionCommon, Tensor } from 'onnxruntime-common'
import invariant from 'tiny-invariant'
import { FileUtils, InferenceSession, defaultModels } from '#common/backend'
import type { Dictionary, Line, LineImage, ModelBaseConstructorArg, ModelCreateOptions } from '#common/types'
import { ModelBase } from './ModelBase'

export class Recognition extends ModelBase {
  #dictionary: Dictionary

  static async create({ models, onnxOptions = {}, ...restOptions }: ModelCreateOptions) {
    const recognitionPath = models?.recognitionPath || defaultModels?.recognitionPath
    invariant(recognitionPath, 'recognitionPath is required')
    const dictionaryPath = models?.dictionaryPath || defaultModels?.dictionaryPath
    invariant(dictionaryPath, 'dictionaryPath is required')
    const model = await InferenceSession.create(recognitionPath, onnxOptions)
    const dictionaryText = await FileUtils.read(dictionaryPath)
    const dictionary = [...dictionaryText.split('\n'), ' ']
    return new Recognition({ model, options: restOptions }, dictionary)
  }

  constructor(options: ModelBaseConstructorArg, dictionary: Dictionary) {
    super(options)
    this.#dictionary = dictionary
  }

  async run(lineImages: LineImage[], { onnxOptions = {} }: { onnxOptions?: InferenceSessionCommon.RunOptions } = {}) {
    const modelDatas = await Promise.all(
      // Detect text from each line image
      lineImages.map(async (lineImage, index) => {
        // Resize Image to 48px height
        //  - height must <= 48
        //  - height: 48 is more accurate then 40, but same as 30
        const image = await (lineImage.image as any).resize({
          height: 48,
        })
        this.debugImage(lineImage.image, `out9-line-${index}.jpg`)
        this.debugImage(image, `out9-line-${index}-resized.jpg`)

        // transform image data to model data
        const modelData = this.imageToInput(image, {
          // mean: [0.5, 0.5, 0.5],
          // std: [0.5, 0.5, 0.5],
        })
        return modelData
      }),
    )

    const allLines: Line[] = []
    // console.time('Recognition')
    for (const modelData of modelDatas) {
      // Run model for each line image
      const output = await this.runModel({ modelData, onnxOptions })
      // use Dictoinary to decode output to text
      const lines = await this.decodeText(output)
      allLines.unshift(...lines)
    }
    // console.timeEnd('Recognition')
    const result = calculateBox({ lines: allLines, lineImages })
    return result
  }

  decodeText(output: Tensor) {
    const data = output
    const predLen = data.dims[2]
    const line: Line[] = []
    let ml = data.dims[0] - 1
    for (let l = 0; l < data.data.length; l += predLen * data.dims[1]) {
      const predsIdx: number[] = []
      const predsProb: number[] = []

      for (let i = l; i < l + predLen * data.dims[1]; i += predLen) {
        const tmpArr = data.data.slice(i, i + predLen) as Float32Array
        const tmpMax = tmpArr.reduce((a, b) => Math.max(a, b), Number.NEGATIVE_INFINITY)
        const tmpIdx = tmpArr.indexOf(tmpMax)
        predsProb.push(tmpMax)
        predsIdx.push(tmpIdx)
      }
      line[ml] = decode(this.#dictionary, predsIdx, predsProb, true)
      ml--
    }
    return line
  }
}

function decode(dictionary: string[], textIndex: number[], textProb: number[], isRemoveDuplicate: boolean) {
  const ignoredTokens = [0]
  const charList = []
  const confList = []
  for (let idx = 0; idx < textIndex.length; idx++) {
    if (textIndex[idx] in ignoredTokens) {
      continue
    }
    if (isRemoveDuplicate) {
      if (idx > 0 && textIndex[idx - 1] === textIndex[idx]) {
        continue
      }
    }
    charList.push(dictionary[textIndex[idx] - 1])
    if (textProb) {
      confList.push(textProb[idx])
    } else {
      confList.push(1)
    }
  }
  let text = ''
  let mean = 0
  if (charList.length) {
    text = charList.join('')
    let sum = 0
    confList.forEach((item) => {
      sum += item
    })
    mean = sum / confList.length
  }
  return { text, mean }
}

function calculateBox({
  lines,
  lineImages,
}: {
  lines: Line[]
  lineImages: LineImage[]
}) {
  let mainLine = lines
  const box = lineImages
  for (const i in mainLine) {
    const b = box[mainLine.length - Number(i) - 1].box
    for (const p of b) {
      p[0] = p[0]
      p[1] = p[1]
    }
    mainLine[i]['box'] = b
  }
  mainLine = mainLine.filter((x) => x.mean >= 0.5)
  mainLine = afAfRec(mainLine)
  return mainLine
}

function afAfRec(lines: Line[]) {
  const outputLines: Line[] = []
  const indexes: Map<BoxType, number> = new Map()
  for (const index in lines) {
    const box: any = lines[index].box
    indexes.set(box, Number(index))
  }

  const groupedBoxes = groupBoxesByMidlineDifference([...indexes.keys()])

  for (const boxes of groupedBoxes) {
    const texts = []
    let mean = 0
    for (const box of boxes) {
      const index = indexes.get(box)
      if (index === undefined) {
        continue
      }
      const line = lines[index]
      texts.push(line.text)
      mean += line.mean
    }
    let outputBox = undefined
    if (boxes.at(0) && boxes.at(-1)) {
      outputBox = [boxes.at(0)![0], boxes.at(-1)![1], boxes.at(-1)![2], boxes.at(0)![3]]
    }
    outputLines.push({
      mean: mean / boxes.length,
      text: texts.join(' '),
      box: outputBox,
    })
  }
  return outputLines
}

function calculateAverageHeight(boxes: BoxType[]): number {
  let totalHeight = 0
  for (const box of boxes) {
    const [[, y1], , [, y2]] = box
    const height = y2 - y1
    totalHeight += height
  }
  return totalHeight / boxes.length
}

function groupBoxesByMidlineDifference(boxes: BoxType[]): BoxType[][] {
  const averageHeight = calculateAverageHeight(boxes)
  const result: BoxType[][] = []
  for (const box of boxes) {
    const [[, y1], , [, y2]] = box
    const midline = (y1 + y2) / 2
    const group = result.find((b) => {
      const [[, groupY1], , [, groupY2]] = b[0]
      const groupMidline = (groupY1 + groupY2) / 2
      return Math.abs(groupMidline - midline) < averageHeight / 2
    })
    if (group) {
      group.push(box)
    } else {
      result.push([box])
    }
  }

  for (const group of result) {
    group.sort((a, b) => {
      const [ltA] = a
      const [ltB] = b
      return ltA[0] - ltB[0]
    })
  }

  result.sort((a, b) => a[0][0][1] - b[0][0][1])

  return result
}

type pointType = [number, number]
type BoxType = [pointType, pointType, pointType, pointType]
