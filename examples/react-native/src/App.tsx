import { useEffect, useState } from 'react'
import { Image, StyleSheet, Text, View } from 'react-native'
import { SafeAreaProvider, SafeAreaView } from 'react-native-safe-area-context'
import { ImagePickerButton } from '#example-react-native/ImagePickerButton'
import { recognize } from '#example-react-native/recognize'
import type { ImageDetails } from '#example-react-native/types'

export default function App() {
  const [image, setImage] = useState<ImageDetails>()
  const [resultText, setResultText] = useState<string>()

  useEffect(() => {
    ;(async function useEffect() {
      if (!image) {
        return
      }
      const resultText = await recognize(image)
      setResultText(resultText)
    })()
  }, [image])

  const handleChange = (image: ImageDetails) => {
    setImage(image)
  }

  return (
    <SafeAreaProvider>
      <SafeAreaView>
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
