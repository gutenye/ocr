import fs from 'node:fs/promises'

const result = await Bun.build({
  entrypoints: ['./ocr.ts'],
  outdir: './build',
  target: 'browser',
  sourcemap: 'external',
  external: ['@techstark/opencv-js'],
})

if (result.success) {
  console.log('Build success')
} else {
  throw new AggregateError(result.logs, 'Build failed')
}

await fs.appendFile('./build/ocr.js', '//# sourceMappingURL=/ocr.js.map')
