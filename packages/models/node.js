import filePath from 'node:path'
import { fileURLToPath } from 'node:url'

export default {
  detectionPath: resolve("./assets/mobile_det.onnx"),
  recognitionPath: resolve("./assets/mobile_rec.onnx"),
  dictionaryPath: resolve("./assets/ppocrv5_dict.txt"),
};

function resolve(path) {
  return filePath.resolve(filePath.dirname(fileURLToPath(import.meta.url)), path)
}
