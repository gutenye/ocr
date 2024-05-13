import Ocr from '@gutenye/ocr-react-native'
import * as FileSystem from 'expo-file-system'
import { useEffect, useState } from 'react'
import { Image, StyleSheet, Text, View } from 'react-native'
import { SafeAreaProvider, SafeAreaView } from 'react-native-safe-area-context'
import { ImagePickerButton } from './ImagePickerButton'
import type { ImageDetails } from './types'

export const DEFAULT_IMAGE = {
  uri: `${FileSystem.bundleDirectory}/gutenye-ocr-react-native.bundle/cn-01.jpg`,
  width: 500,
  height: 500,
}

export default function App() {
  const [ocr, setOcr] = useState<Ocr>()
  const [image, setImage] = useState<ImageDetails>(DEFAULT_IMAGE)
  const [resultText, setResultText] = useState<string>()

  useEffect(() => {
    ;(async () => {
      console.log(':: 1')
      const ocr = await Ocr.create({
        isDebug: true,
      })
      console.log(':: 2')
      setOcr(ocr)
    })()
  }, [])

  useEffect(() => {
    ;(async function useEffect() {
      if (!image || !ocr) {
        return
      }
      console.log(':: 3')
      const lines = await ocr.detect(image.uri)
      console.log(':: 4')
      setResultText(lines.join('\n'))
    })()
  }, [ocr, image])

  const handleChange = (image: ImageDetails) => {
    setImage(image)
  }

  return (
    <SafeAreaProvider>
      <SafeAreaView style={styles.safeAreaView}>
        <View style={styles.container}>
          <ImagePickerButton onChange={handleChange} />
          {image && <Image source={{ uri: image.uri }} style={styles.image} />}
          {resultText && <Text>{resultText}</Text>}
        </View>
      </SafeAreaView>
    </SafeAreaProvider>
  )
}

const styles = StyleSheet.create({
  safeAreaView: {
    flex: 1,
  },
  container: {
    flex: 1,
    // alignItems: 'center',
    // justifyContent: 'center',
  },
  image: {
    aspectRatio: 1,
    width: '100%',
    objectFit: 'contain',
  },
})
