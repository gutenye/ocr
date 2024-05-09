#!/usr/bin/env bash

cat ../../models/ppocr_keys_v1.txt

bun run build.ts

cp -rL build build2