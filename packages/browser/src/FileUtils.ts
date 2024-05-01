import { FileUtilsBase } from '@gutenye/ocr-common'

export class FileUtils extends FileUtilsBase {
  static async read(url: string) {
    const res = await fetch(url)
    return await res.text()
  }
}
