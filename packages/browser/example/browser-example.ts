import Ocr from '@gutenye/ocr-browser'
import * as ort from 'onnxruntime-web'

// Configure ONNX Runtime for large models
ort.env.wasm.numThreads = 1 // Disable threading to save memory
ort.env.wasm.simd = true
// ort.env.wasm.proxy = false // Disable worker to save memory

async function main() {
  try {
    const ocr = await Ocr.create({
      isDebug: true,
      detectionThreshold: 0.3,
      boxThreshold: 0.6,
      unclipRatio: 1.5,
      recognitionThreshold: 0.5,
      onnxOptions: {
        executionProviders: ['wasm'],
        graphOptimizationLevel: 'all',
      },
      models: {
        detectionPath: '/assets/PP-OCRv5_server_det_infer.onnx',
        recognitionPath: '/assets/PP-OCRv5_server_rec_infer.onnx',
        dictionaryPath: '/assets/ppocr_keys_v5.txt',
      },
    })
  } catch (error) {
    console.error('Failed to create OCR:', error)
    document.querySelector('#title')!.textContent = 'OCR Failed to Load'
    const resultText = document.querySelector('#result-text')
    if (resultText) {
      resultText.textContent = `Error: ${error instanceof Error ? error.message : String(error)}`
    }
    throw error
  }
  const hideElement = document.querySelector('.hide')
  if (hideElement) {
    (hideElement as HTMLElement).style.visibility = 'visible'
  }
  document.querySelector('#title')!.textContent = 'OCR is ready'

  createApp(async ({ imageUrl }) => {
    const startTime = new Date().valueOf()
    const result = await ocr.detect(imageUrl)
    const duration = new Date().valueOf() - startTime

    return {
      text: result.map((v) => `${v.mean.toFixed(2)} ${v.text}`).join('\n'),
      duration,
    }
  })
}

function createApp(
  runOcr: (params: {
    imageUrl: string
  }) => Promise<{ text: string; duration: number }>,
) {
  const resultTextEl = document.querySelector('#result-text') as HTMLDivElement
  const performanceEl = document.querySelector('#performance') as HTMLDivElement
  const resultImageEl = document.querySelector('#result-image') as HTMLImageElement
  const inputImageEl = document.querySelector('#input-image') as HTMLImageElement
  inputImageEl.addEventListener('change', async (event) => {
    const file = (event.target as HTMLInputElement).files?.[0]
    if (!file) {
      return
    }
    const imageUrl = URL.createObjectURL(file)
    await handleChange(imageUrl)
  })
  const handleChange = async (imageUrl: string) => {
    document.querySelectorAll('canvas').forEach((el) => el.remove())
    resultTextEl.textContent = 'Working in progress...'
    resultImageEl.setAttribute('src', imageUrl)
    const { text, duration } = await runOcr({ imageUrl })
    resultTextEl.textContent = text
    performanceEl.textContent = `Performance: ${duration}ms (Close Chrome DevTools to get accureate result)`
  }

  if (process.env.DEFAULT_IMAGE_PATH) {
    handleChange(process.env.DEFAULT_IMAGE_PATH)
  }
}

main()
