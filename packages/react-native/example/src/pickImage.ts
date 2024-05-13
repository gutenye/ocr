import * as ImagePicker from 'expo-image-picker'
import type { ImageDetails } from './types'

export async function pickImage() {
  // console.log(':: permissin', await ImagePicker.getMediaLibraryPermissionsAsync())
  // console.log(':: request', await ImagePicker.requestMediaLibraryPermissionsAsync())

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
