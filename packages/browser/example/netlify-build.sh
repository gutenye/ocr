#!/usr/bin/env bash
# Build script for browser example with v5 models

export DEFAULT_IMAGE_PATH='/assets/cn-01.jpg'

# Build the common package first (dependency)
cd ../../common
bun run tsc --project tsconfig.build.json
bun run tsc-alias --project tsconfig.build.json

# Build the browser package
cd ../browser
bun run tsc --project tsconfig.build.json
bun run tsc-alias --project tsconfig.build.json

# Build the example
cd example
bun run build.ts

cp -rL build build2