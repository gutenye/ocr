import Ocr from '@gutenye/ocr-react-native'
import * as FileSystem from 'expo-file-system'
import { useEffect, useState } from 'react'
import { Image, StyleSheet, Text, View } from 'react-native'
import { SafeAreaProvider, SafeAreaView } from 'react-native-safe-area-context'
import { ImagePickerButton } from './ImagePickerButton'
// import { recognize } from './recognize'
import type { ImageDetails } from './types'

async function main() {
  const ocr = await Ocr.create()
  const result = await ocr.detect(`${FileSystem.bundleDirectory}/gutenye-ocr-react-native.bundle/cn-01.jpg`, {
    number1: 1,
    number2: 0,
    float1: 1.1,
    string: 'a',
    boolean1: true,
    boolean2: false,
  })
  console.log('js', result)
  return result
}
main()

export default function App() {
  const [image, setImage] = useState<ImageDetails>()
  const [resultText, setResultText] = useState<string>()

  useEffect(() => {
    ;(async function useEffect() {
      if (!image) {
        return
      }
      // const resultText = await recognize(image)
      // setResultText(resultText)
    })()
  }, [image])

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
    alignItems: 'center',
    justifyContent: 'center',
  },
  image: {
    aspectRatio: 1,
    width: '100%',
    objectFit: 'contain',
  },
})
