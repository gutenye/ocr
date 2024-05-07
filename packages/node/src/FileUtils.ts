import fs from 'node:fs/promises'
import { FileUtilsBase } from '@gutenye/ocr-common'

export class FileUtils extends FileUtilsBase {
  static async read(path: string) {
    return await fs.readFile(path, 'utf8')
  }
}
