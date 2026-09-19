# Guten OCR

> [Demo](https://gutenye-ocr.netlify.app/) | [Roadmap](https://github.com/users/gutenye/projects/5/views/4)

**an OCR Javascript library runs on Node.js, Browser, React Native and C++** 

Based on [PaddleOCR](https://github.com/PaddlePaddle/PaddleOCR) and [ONNX Runtime](https://github.com/microsoft/onnxruntime), supports PP-OCRv4 and PP-OCRv5 models

## Getting Started

### Node

> [Example](./packages/node/example/README.md)

```ts
bun add @gutenye/ocr-node @gutenye/ocr-models
import Ocr from '@gutenye/ocr-node'
const ocr = await Ocr.create()
const result = await ocr.detect('a.jpg')
```

Defaults to the PP-OCRv5 models from `@gutenye/ocr-models`. Pass `models` to use your own.

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

The browser build has no default models, so `models` is required and the files must be served by your
app. The PP-OCRv5 server models are ~85MB each, which is usually too large to download in a browser —
prefer PP-OCRv4 here and use PP-OCRv5 on Node, where it is the default.

### React Native

> [Example](./packages/react-native/example/README.md)

```ts
bun add @gutenye/ocr-react-native
import Ocr from '@gutenye/ocr-react-native'
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
  detectionThreshold?: number // Default: 0.3. Node and Browser
  boxThreshold?: number // Default: 0.6. Node and Browser
  unclipRatio?: number // Default: 1.5. Node and Browser
  recognitionThreshold?: number // Default: 0.5. Node and Browser
  recognitionImageMaxSize?: number // RN only
  detectionBoxThreshold?: number // RN only
  detectionUnclipRatiop?: number // RN only
  detectionUseDilate?: boolean // RN only
  detectionUsePolygonScore?: boolean // RN only
  useDirectionClassify?: boolean // RN only
  onnxOptions?: {}       // Node and Browser. Pass to ONNX Runtime
}): Promise<Ocr>

ocr.detect(imagePath: string | {data: Uint8Array | Uint8ClampedArray | Buffer, width: number, height: number}, {
  onnxOptions?: {}     // Node only. Pass to ONNX Runtime
}): Promise<{texts: TextLine[], resizedImageWidth: number, resizedImageHeight: number}>

TextLine {
  text: string
  score: number
  frame: { top, left, width, height }
}

```

## Development

The model assets are **not** stored in this repository, so the clone is small and no Git LFS is needed.

```sh
git clone git@github.com:gutenye/ocr.git
cd ocr
bun install
packages/models/ake fetch
```

`ake fetch` downloads every model into `packages/models/assets` and verifies each file against the
sha256 in `packages/models/assets.json`. Pass a comma-separated list to fetch only some of them, and
run `ake verify` to re-check what is already on disk. The example `ake start` commands fetch the
models they need on their own.

To mirror the assets yourself, host the files listed in `assets.json` under one URL prefix and set
`GUTEN_OCR_MODELS_BASE_URL` to it.

- [Development](docs/Development.md)

## Related Projects

| Name                                                           | Platforms | Note                            |
| -------------------------------------------------------------- | --------- | ------------------------------- |
| [eSearch-OCR](https://github.com/xushengfeng/eSearch-OCR)      | Electron  |                                 |
| [paddleocr-onnx](https://github.com/backrunner/paddleocr-onnx) | Node      | Recogination part is incomplete |
| [ocrjs](https://github.com/SOVLOOKUP/ocrjs)                    | Node      | Recogination part is incomplete |
| [Paddle-Lite-Demo](https://github.com/PaddlePaddle/Paddle-Lite-Demo) | Mobile, C++ | |

## License

The source code of this project is licensed under the [MIT](./LICENSE) license.

The model and dictionary assets listed in `packages/models/assets.json` are **not** covered by the MIT license. They are downloaded separately, come from [PaddleOCR](https://github.com/PaddlePaddle/PaddleOCR), and remain under its [Apache-2.0](https://github.com/PaddlePaddle/PaddleOCR/blob/main/LICENSE) license. Converting a model to ONNX does not change the license of its weights.
