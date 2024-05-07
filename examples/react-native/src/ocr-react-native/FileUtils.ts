import { FileUtilsBase } from '@gutenye/ocr-common'
import { readAsStringAsync } from 'expo-file-system'

export class FileUtils extends FileUtilsBase {
  static async read(path: string) {
    return readAsStringAsync(path)
  }
}
