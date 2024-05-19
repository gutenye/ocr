import { NativeModules, Platform } from 'react-native'
import pkg from '../package.json'
import type { OcrOptions } from './types'

const LINKING_ERROR = `
The package '${pkg.name}' doesn't seem to be linked. Make sure:
${Platform.select({ ios: `- You have run 'pod install'`, default: '' })} 
- You rebuilt the app after installing the package
- You are not using Expo Go
`.trim()

// @ts-expect-error
const isTurboModuleEnabled = global.__turboModuleProxy != null

const OcrModule = isTurboModuleEnabled ? require('./NativeOcr').default : NativeModules.RNOcr

const Ocr = OcrModule
  ? OcrModule
  : new Proxy(
      {},
      {
        get() {
          throw new Error(LINKING_ERROR)
        },
      },
    )

// @ts-ignore
const jsiModule: {
  create(options: OcrOptions): Promise<void>
} = global

if (Platform.OS === 'android' && !jsiModule.create) {
  throw new Error(`${pkg.name}: failed to load JSI module`)
}

const module = Platform.OS === 'ios' ? Ocr : jsiModule

export default module
