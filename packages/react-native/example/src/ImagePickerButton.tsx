import { Button } from 'react-native'
import { pickImage } from '#example-react-native/pickImage'
import type { ImageDetails } from '#example-react-native/types'

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
