import filePath from 'node:path'
import { fileURLToPath } from 'node:url'
import { Ocr } from './Ocr'

export default function useDefaultModels() {
  Ocr.DEFAULT_DETECTION_PATH = resolve('../assets/ch_PP-OCRv4_det_infer.onnx')
  Ocr.DEFAULT_RECOGINTION_PATH = resolve('../assets/ch_PP-OCRv4_rec_infer.onnx')
  Ocr.DEFAULT_DICTIONARY_PATH = resolve('../assets/ppocr_keys_v1.txt')
}

function resolve(path: string) {
  return filePath.resolve(
    filePath.dirname(fileURLToPath(import.meta.url)),
    path,
  )
}
