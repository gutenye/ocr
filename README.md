# @gutenye/ocr

> an OCR Javascript library runs on Node.js, Browser and React Native

| [Demo](https://gutenye-ocr.netlify.app/) | 

Based on [PaddleOCR](https://github.com/PaddlePaddle/PaddleOCR), supports PP-OCRv4 model

## Examples

- [Node Example](./examples/node/README.md)
- [Browser Example](./examples/browser/README.md)

## Getting Started

### Node

```ts
bun add @gutenye/ocr-node
import Ocr from '@guenye/ocr-node'
const ocr = await Ocr.create()
const result = await ocr.detect('a.jpg')
```

### Browser

```ts
bun add @gutenye/ocr-browser
import Ocr from '@gutenye/ocr-browser'
const ocr = await Ocr.create({
  models: {
    detectionPath: '/assets/ch_PP-OCRv4_det_infer.onnx',
    recognitionPath: '/assets/ch_PP-OCRv4_rec_infer.onnx',
    dictionaryPath: '/assets/ppocr_keys_v1.txt'
  }
})
const result = await ocr.detect(imageUrl)
```

### React Native

Due to opencv does not work on React Native. You need to implement the [detectBoxes](./packages/common/src/splitIntoLineImages.ts) method by yourself

## Related Projects

| Name                                                           | Platforms | Note                            |
| -------------------------------------------------------------- | --------- | ------------------------------- |
| [eSearch-OCR](https://github.com/xushengfeng/eSearch-OCR)      | Electron  |                                 |
| [paddleocr-onnx](https://github.com/backrunner/paddleocr-onnx) | Node      | Recogination part is incomplete |
| [ocrjs](https://github.com/SOVLOOKUP/ocrjs)                    | Node      | Recogination part is incomplete |


