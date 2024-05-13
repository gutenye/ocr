import Ocr from '@gutenye/ocr-react-native'
import * as FileSystem from 'expo-file-system'
import { useEffect, useState } from 'react'
import { Image, StyleSheet, Text, View } from 'react-native'
import { SafeAreaProvider, SafeAreaView } from 'react-native-safe-area-context'
import { ImagePickerButton } from './ImagePickerButton'
import type { ImageDetails } from './types'

const DEFAULT_IMAGE = `${FileSystem.bundleDirectory}/gutenye-ocr-react-native.bundle/cn-01.jpg`
// const DEFAULT_IMAGE = undefined

export default function App() {
  const [ocr, setOcr] = useState<Ocr>()
  const [imagePath, setImagePath] = useState<string | undefined>(DEFAULT_IMAGE)
  const [resultText, setResultText] = useState<string>()

  useEffect(() => {
    ;(async () => {
      const ocr = await Ocr.create({
        isDebug: true,
      })
      setOcr(ocr)
    })()
  }, [])

  useEffect(() => {
    ;(async function useEffect() {
      if (!imagePath || !ocr) {
        return
      }
      const lines = await ocr.detect(imagePath)
      setResultText(lines.join('\n'))
    })()
  }, [ocr, imagePath])

  const handleChange = (image: ImageDetails) => {
    setImagePath(image.uri)
  }

  return (
    <SafeAreaProvider>
      <SafeAreaView style={styles.safeAreaView}>
        <View style={styles.container}>
          <ImagePickerButton onChange={handleChange} />
          {imagePath && <Image source={{ uri: imagePath }} style={styles.image} />}
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
