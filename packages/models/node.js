import filePath from 'node:path'
import { fileURLToPath } from 'node:url'

export default {
  detectionPath: resolve('./assets/PP-OCRv5_server_det_infer.onnx'),
  recognitionPath: resolve('./assets/PP-OCRv5_server_rec_infer.onnx'),
  dictionaryPath: resolve('./assets/ppocr_keys_v5.txt'),
}

function resolve(path) {
  return filePath.resolve(filePath.dirname(fileURLToPath(import.meta.url)), path)
}
