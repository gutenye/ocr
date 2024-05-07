import fs from 'node:fs/promises'
import { $ } from 'bun'

const result = await Bun.build({
  entrypoints: ['./browser.ts'],
  outdir: './build',
  target: 'browser',
  sourcemap: 'external',
  define: {
    'process.env.DEFAULT_IMAGE_PATH': process.env.DEFAULT_IMAGE_PATH || '',
  },
})

if (result.success) {
  console.log('Build success')
} else {
  throw new AggregateError(result.logs, 'Build failed')
}

await fs.appendFile('./build/browser.js', '//# sourceMappingURL=browser.js.map')

await $`cd build; ln -sf ../../assets .`
await $`cd build; ln -sf ../{index.html,index.css} .`
await $`cd build; ln -sf ../../../node_modules/onnxruntime-web/dist/*.wasm .`
