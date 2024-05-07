import fs from 'node:fs/promises'

const result = await Bun.build({
  entrypoints: ['../../packages/react-native/src/index.ts'],
  outdir: './build',
  target: 'node',
  bundler
  sourcemap: 'external',
})

if (result.success) {
  console.log('Build success')
} else {
  throw new AggregateError(result.logs, 'Build failed')
}

await fs.appendFile('./build/index.js', '//# sourceMappingURL=index.js.map')
