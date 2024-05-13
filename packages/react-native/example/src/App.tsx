import Ocr from '@gutenye/ocr-react-native'
import * as FileSystem from 'expo-file-system'
import { useEffect, useState } from 'react'
import { Image, StyleSheet, Text, View } from 'react-native'
import { SafeAreaProvider, SafeAreaView } from 'react-native-safe-area-context'
import { ImagePickerButton } from './ImagePickerButton'
import type { ImageDetails } from './types'

//  /private/var/containers/Bundle/Application/55E249DC-B95F-4B05-9A10-6C192143E8EB/GutenOCRExample.app//guten-ocr.bundle/cn-01.jpg
// 'file:///var/mobile/Containers/Data/Application/AA3D33D1-DAB1-43B8-BD9D-D72F1AE89A6F/Library/Caches/ImagePicker/43FDA18A-DB48-4436-B231-1B880CA9BBDB.jpg'

const DEFAULT_IMAGE = `${FileSystem.bundleDirectory}/guten-ocr.bundle/cn-01.jpg1`
// const DEFAULT_IMAGE = undefined

export default function App() {
  const [ocr, setOcr] = useState<Ocr>()
  const [imagePath, setImagePath] = useState<string | undefined>(DEFAULT_IMAGE)
  const [resultText, setResultText] = useState<string>()

  useEffect(() => {
    ;(async () => {
      const ocr = await Ocr.create({
        isDebug: true,
        recognitionImageMaxSize: 2048,
      })
      setOcr(ocr)
    })()
  }, [])

  useEffect(() => {
    ;(async function useEffect() {
      if (!imagePath || !ocr) {
        return
      }
      try {
        const newImagePath = imagePath.replace('file://', '')
        const lines = await ocr.detect(newImagePath)
        setResultText(lines.join('\n'))
      } catch (error) {
        if (error instanceof Error) {
          setResultText(error.message)
        }
      }
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
