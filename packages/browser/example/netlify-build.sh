#!/usr/bin/env bash
set -euo pipefail

export DEFAULT_IMAGE_PATH='/assets/cn-01.jpg'

# The models are not stored in git, download them into packages/models/assets
cd ../../models
./ake fetch ch_PP-OCRv4_det_infer.onnx,ch_PP-OCRv4_rec_infer.onnx,ppocr_keys_v1.txt

# Build the common package first (dependency)
cd ../common
bun run tsc --project tsconfig.build.json
bun run tsc-alias --project tsconfig.build.json

# Build the browser package
cd ../browser
bun run tsc --project tsconfig.build.json
bun run tsc-alias --project tsconfig.build.json

# Build the example
cd example
bun run build.ts

# Copy build output and assets
cp -rL build build2
cp -rL assets build2/assets