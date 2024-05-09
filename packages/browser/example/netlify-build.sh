#!/usr/bin/env bash

export DEFAULT_IMAGE_PATH='/assets/cn-01.jpg'

bun run build.ts

cp -rL build build2