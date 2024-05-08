import { Button } from 'react-native'
import { pickImage } from './pickImage'
import type { ImageDetails } from './types'

export function ImagePickerButton({
  onChange,
}: {
  onChange: (image: ImageDetails) => void
}) {
  const handlePress = async () => {
    const image = await pickImage()
    if (image) {
      onChange(image)
    }
  }

  return <Button title="Pick an image" onPress={handlePress} />
}
