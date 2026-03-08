import filePath from 'node:path'
import { fileURLToPath } from 'node:url'

export default {
  // v5 models (default)
  detectionPath: resolve('./assets/PP-OCRv5_server_det_infer.onnx'),
  recognitionPath: resolve('./assets/PP-OCRv5_server_rec_infer.onnx'),
  dictionaryPath: resolve('./assets/ppocr_keys_v5.txt'),

  // v4 models (legacy)
  detectionPathV4: resolve('./assets/ch_PP-OCRv4_det_infer.onnx'),
  recognitionPathV4: resolve('./assets/ch_PP-OCRv4_rec_infer.onnx'),
  dictionaryPathV4: resolve('./assets/ppocr_keys_v1.txt'),
}

function resolve(path) {
  return filePath.resolve(filePath.dirname(fileURLToPath(import.meta.url)), path)
}
