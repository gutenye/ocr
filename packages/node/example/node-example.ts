import Ocr from '@gutenye/ocr-node'

async function main() {
  const ocr = await Ocr.create({
    isDebug: true,
    debugOutputDir: './output',
    onnxOptions: {
      // logSeverityLevel: 0,
    },
  })
  const imagePath = process.argv.slice(2)[0]
  // console.time('ocr')
  const result = await ocr.detect(imagePath)
  // console.timeEnd('ocr')

  console.log(result.texts.map((v) => `${v.mean.toFixed(2)} ${v.text}`).join('\n'))
}

main()
