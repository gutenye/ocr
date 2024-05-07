import * as ImagePicker from 'expo-image-picker'
import type { ImageDetails } from '#example-react-native/types'

export async function pickImage() {
  const result = await ImagePicker.launchImageLibraryAsync()
  if (result.canceled) {
    return
  }
  const asset = result.assets[0]
  const image: ImageDetails = {
    uri: asset.uri,
    width: asset.width,
    height: asset.height,
  }
  return image
}
