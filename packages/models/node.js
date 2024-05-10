import filePath from 'node:path'
import { fileURLToPath } from 'node:url'

export default {
  detectionPath: resolve('./assets/ch_PP-OCRv4_det_infer.onnx'),
  recognitionPath: resolve('./assets/ch_PP-OCRv4_rec_infer.onnx'),
  dictionaryPath: resolve('./assets/ppocr_keys_v1.txt'),
}

function resolve(path) {
  return filePath.resolve(filePath.dirname(fileURLToPath(import.meta.url)), path)
}
