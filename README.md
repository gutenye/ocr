# Guten OCR

> [Demo](https://gutenye-ocr.netlify.app/) | [Roadmap](https://github.com/users/gutenye/projects/5/views/4)

**an OCR Javascript library runs on Node.js, Browser, React Native and C++** 

Based on [PaddleOCR](https://github.com/PaddlePaddle/PaddleOCR) and [ONNX Runtime](https://github.com/microsoft/onnxruntime), supports PP-OCRv4 model

## Getting Started

### Node

> [Example](./packages/node/example/README.md)

```ts
bun add @gutenye/ocr-node
import Ocr from '@gutenye/ocr-node'
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
NativeOcr* ocr = new NativeOcr(..)
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
  debugOutputDir?: string // Node only
  recognitionImageMaxSize?: number // RN only
  detectionThreshold?: number // RN only
  detectionBoxThreshold?: number // RN only
  detectionUnclipRatiop?: number // RN only
  detectionUseDilate?: boolean // RN only
  detectionUsePolygonScore?: boolean // RN only
  useDirectionClassify?: boolean // RN only
  onnxOptions?: {}       // Node only. Pass to ONNX Runtime
}): Promise<Ocr>

ocr.detect(imagePath, { 
  onnxOptions?: {}     // Node only. Pass to ONNX Runtime
}): Promise<TextLine[]>

TextLine {
  text: string
  score: number
  frame: { top, left, width, height }
}

```

## Development

- Requires Git LFS to clone the repo

```sh
brew install git-lfs 
git clone git@github.com:gutenye/ocr.git
```

- [Development](docs/Development.md)

## Related Projects

| Name                                                           | Platforms | Note                            |
| -------------------------------------------------------------- | --------- | ------------------------------- |
| [eSearch-OCR](https://github.com/xushengfeng/eSearch-OCR)      | Electron  |                                 |
| [paddleocr-onnx](https://github.com/backrunner/paddleocr-onnx) | Node      | Recogination part is incomplete |
| [ocrjs](https://github.com/SOVLOOKUP/ocrjs)                    | Node      | Recogination part is incomplete |
| [Paddle-Lite-Demo](https://github.com/PaddlePaddle/Paddle-Lite-Demo) | Mobile, C++ | |
