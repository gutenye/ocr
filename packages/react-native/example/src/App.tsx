import Ocr from '@gutenye/ocr-react-native'
import * as FileSystem from 'expo-file-system'
import { useEffect, useState } from 'react'
import { Image, Platform, ScrollView, StyleSheet, Text, View } from 'react-native'
import { SafeAreaProvider, SafeAreaView } from 'react-native-safe-area-context'
import { ImagePickerButton } from './ImagePickerButton'
import type { ImageDetails } from './types'

const isIos = Platform.OS === 'ios'

const DEFAULT_IMAGE = `${isIos ? FileSystem.bundleDirectory : FileSystem.cacheDirectory}/guten-ocr.bundle/cn-01.jpg`
const OUTPUT_DIR = `${FileSystem.cacheDirectory}/guten-ocr.outputs`
const IS_DEBUG = true

export default function App() {
  const [ocr, setOcr] = useState<Ocr>()
  const [imagePath, setImagePath] = useState<string | undefined>(DEFAULT_IMAGE)
  const [resultLines, setResultLines] = useState<string[]>([])

  useEffect(() => {
    ;(async () => {
      const ocr = await Ocr.create({
        isDebug: IS_DEBUG,
        // recognitionImageMaxSize: -1,
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
        setResultLines([])
        const lines = await ocr.detect(imagePath)
        setResultLines(lines)
      } catch (error) {
        if (error instanceof Error) {
          setResultLines([error.message])
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
        <ScrollView>
          <ImagePickerButton onChange={handleChange} />
          {imagePath && <Image source={{ uri: imagePath }} style={styles.image} />}
          {resultLines.length > 0 && <Text>{resultLines.join('\n')}</Text>}
          {IS_DEBUG && resultLines.length > 0 && (
            <>
              <Image source={{ uri: `${OUTPUT_DIR}/boxes.jpg` }} style={styles.image} />
              {resultLines.map((_, index) => (
                <View key={index}>
                  {/* <Image
                    source={{ uri: `${OUTPUT_DIR}/line-${index}-resized.jpg` }}
                    style={styles.lineImage}
                  /> */}
                </View>
              ))}
            </>
          )}
        </ScrollView>
      </SafeAreaView>
    </SafeAreaProvider>
  )
}

const styles = StyleSheet.create({
  safeAreaView: {
    flex: 1,
  },
  image: {
    aspectRatio: 1,
    width: '100%',
    objectFit: 'contain',
  },
  lineImage: {
    height: 32,
    objectFit: 'contain',
  },
})
