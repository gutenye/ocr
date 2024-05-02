import Ocr from '@gutenye/ocr-node'
import fs from 'fs/promises'
import filePath from 'path'

async function main() {
  const ocr = await Ocr.create()
  const imagePath = process.argv.slice(2)[0]
  await fs.mkdir('output', { recursive: true })
  console.time('ocr')
  const result = await ocr.detect(imagePath, { isDebug: true })
  console.log(result)
  console.timeEnd('ocr')

  console.log(result.map((v) => `${v.mean.toFixed(2)} ${v.text}`).join('\n'))

  function r(path: string) {
    return filePath.resolve(__dirname, path)
  }
}

main()
