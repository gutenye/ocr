import { FileUtilsBase } from '@gutenye/ocr-common'
import fs from 'fs/promises'

export class FileUtils extends FileUtilsBase {
  static async read(path: string) {
    return await fs.readFile(path, 'utf8')
  }
}
