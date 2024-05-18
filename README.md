# Guten OCR

> [Demo](https://gutenye-ocr.netlify.app/)

**an OCR Javascript library runs on Node.js, Browser, React Native and C++** 

Based on [PaddleOCR](https://github.com/PaddlePaddle/PaddleOCR), supports PP-OCRv4 model

## Getting Started

### Node

> [Example](./packages/node/example/README.md)

```ts
bun add @gutenye/ocr-node
import Ocr from '@guenye/ocr-node'
const ocr = await Ocr.create()
const result = await ocr.detect('a.jpg')
```

### Browser

> [Example](./packages/browser/example/README.md)

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
const result = await ocr.detect('/a.jpg')
```

### React Native

> [Example](./packages/react-native/example/README.md)

```ts
bun add @gutenye/ocr-react-native
import Ocr from '@guenye/ocr-react-native'
const ocr = await Ocr.create()
const result = await ocr.detect('a.jpg')
```

### C++

> [Example](./packages/react-native/cpp/example/README.md)

```cpp
#include "native-ocr.h"
NativeOcr* ocr = new NativeOcr(RawOptions {
  {"detectionModelPath", "./assets/ch_PP-OCRv4_det_infer.onnx"},
  {"recognitionModelPath", "./assets/ch_PP-OCRv4_rec_infer.onnx"},
  {"dictionaryPath", "./assets/ppocr_keys_v1.txt"}
});
auto result = ocr->detect("a.jpg");
```

### API Reference

```ts
Ocr.create({
  models?: {
    detectionPath: string
    recognitionPath: string
    dictionaryPath: string
  },
  isDebug?: boolean
  recognitionImageMaxSize?: number 
  detectionThreshold?: number
  detectionBoxThreshold?: number
  detectionUnclipRatiop?: number
  detectionUseDilate?: boolean
  detectionUsePolygonScore?: boolean
  detectionUseDirectionClassify?: boolean
  onnxOptions?: {}       // Pass to ONNX Runtime
}): Promise<Ocr>

ocr.detect(imagePath, { 
  onnxOptions?: {}     // Pass to ONNX Runtime
}): Promise<Result>
```

## Development

- [Development](docs/Development.md)


## Related Projects

| Name                                                           | Platforms | Note                            |
| -------------------------------------------------------------- | --------- | ------------------------------- |
| [eSearch-OCR](https://github.com/xushengfeng/eSearch-OCR)      | Electron  |                                 |
| [paddleocr-onnx](https://github.com/backrunner/paddleocr-onnx) | Node      | Recogination part is incomplete |
| [ocrjs](https://github.com/SOVLOOKUP/ocrjs)                    | Node      | Recogination part is incomplete |
| [Paddle-Lite-Demo](https://github.com/PaddlePaddle/Paddle-Lite-Demo) | Mobile, C++ | |
