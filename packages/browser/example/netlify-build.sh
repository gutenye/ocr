#!/usr/bin/env bash

export DEFAULT_IMAGE_PATH = '/assets/cn-01.jpg'

cat ../../models/ppocr_keys_v1.txt

bun run build.ts

cp -rL build build2