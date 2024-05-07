import Ocr from '#example-react-native/ocr-react-native'
import type { ImageDetails } from '#example-react-native/types'

export async function recognize(image: ImageDetails) {
  const ocr = await Ocr.create()
  console.time('ocr')
  const result = await ocr.detect(image.uri)
  console.timeEnd('ocr')
  const resultText = result.map((v) => `${v.mean.toFixed(2)} ${v.text}`).join('\n')
  return resultText
}
