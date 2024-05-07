// import Ocr from './ocr-react-native'
// import Ocr from '@gutenye/ocr-react-native'
import Ocr from '@gutenye/ocr-common'
import camera from 'expo-camera'
import { StatusBar } from 'expo-status-bar'
import { StyleSheet, Text, View } from 'react-native'
import { SafeAreaProvider, SafeAreaView } from 'react-native-safe-area-context'

console.log(':: 1', Ocr)

export default function App() {
  return (
    <SafeAreaProvider>
      <SafeAreaView>
        <View style={styles.container}>
          <Text>2Open up App.tsx to start working on your app!</Text>
          <StatusBar style="auto" />
        </View>
      </SafeAreaView>
    </SafeAreaProvider>
  )
}

const styles = StyleSheet.create({
  container: {
    // flex: 1,
    backgroundColor: '#fff',
    alignItems: 'center',
    justifyContent: 'center',
  },
})
