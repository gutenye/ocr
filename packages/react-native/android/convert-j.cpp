#include "convert-j.h"
#include <android/log.h>

#define LOG_TAG "RNOcrModule"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// jstring -> std::string
std::string convertJstring(JNIEnv* env, jstring jvalue) {
  const char* charPointer = env->GetStringUTFChars(jvalue, JNI_FALSE);
  auto stdValue = std::string(charPointer);
  env->ReleaseStringUTFChars(jvalue, charPointer);
  return stdValue;
}

// ReadableArray -> std::vector<std::any>
std::vector<std::any> convertReadableArray(JNIEnv* env, jobject readableArray) {
  std::vector<std::any> result;

  // Get the required class and methods
  jclass readableArrayClass = env->FindClass("com/facebook/react/bridge/ReadableArray");
  jmethodID sizeMethod = env->GetMethodID(readableArrayClass, "size", "()I");
  jmethodID getTypeMethod =
      env->GetMethodID(readableArrayClass, "getType", "(I)Lcom/facebook/react/bridge/ReadableType;");
  jmethodID getStringMethod = env->GetMethodID(readableArrayClass, "getString", "(I)Ljava/lang/String;");
  jmethodID getDoubleMethod = env->GetMethodID(readableArrayClass, "getDouble", "(I)D");
  jmethodID getBooleanMethod = env->GetMethodID(readableArrayClass, "getBoolean", "(I)Z");
  jmethodID getMapMethod = env->GetMethodID(readableArrayClass, "getMap", "(I)Lcom/facebook/react/bridge/ReadableMap;");
  jmethodID getArrayMethod =
      env->GetMethodID(readableArrayClass, "getArray", "(I)Lcom/facebook/react/bridge/ReadableArray;");

  jint size = env->CallIntMethod(readableArray, sizeMethod);

  for (jint i = 0; i < size; ++i) {
    // type = getType(readableArray[i])
    jobject typeObject = env->CallObjectMethod(readableArray, getTypeMethod, i);
    jstring typeString = (jstring)env->CallObjectMethod(
        typeObject, env->GetMethodID(env->FindClass("java/lang/Enum"), "name", "()Ljava/lang/String;"));
    const char* type = env->GetStringUTFChars(typeString, JNI_FALSE);

    if (strcmp(type, "String") == 0) {
      jstring value = (jstring)env->CallObjectMethod(readableArray, getStringMethod, i);
      result.push_back(convertJstring(env, value));
    } else if (strcmp(type, "Number") == 0) {
      double value = env->CallDoubleMethod(readableArray, getDoubleMethod, i);
      result.push_back(value);
    } else if (strcmp(type, "Boolean") == 0) {
      bool value = env->CallBooleanMethod(readableArray, getBooleanMethod, i);
      result.push_back(value);
    } else if (strcmp(type, "Map") == 0) {
      jobject nestedMap = env->CallObjectMethod(readableArray, getMapMethod, i);
      result.push_back(convertReadableMap(env, nestedMap));
    } else if (strcmp(type, "Array") == 0) {
      jobject nestedArray = env->CallObjectMethod(readableArray, getArrayMethod, i);
      result.push_back(convertReadableArray(env, nestedArray));
    }

    env->ReleaseStringUTFChars(typeString, type);
  }

  return result;
}

// ReadableMap -> std::unordered_map<std::string, std::any>
std::unordered_map<std::string, std::any> convertReadableMap(JNIEnv* env, jobject readableMap) {
  std::unordered_map<std::string, std::any> result;

  // Get the ReadableMap class and required methods
  jclass readableMapClass = env->FindClass("com/facebook/react/bridge/ReadableMap");
  jclass readableMapKeySetIteratorClass = env->FindClass("com/facebook/react/bridge/ReadableMapKeySetIterator");
  jmethodID keySetIteratorMethod =
      env->GetMethodID(readableMapClass, "keySetIterator", "()Lcom/facebook/react/bridge/ReadableMapKeySetIterator;");
  jmethodID hasNextKeyMethod = env->GetMethodID(readableMapKeySetIteratorClass, "hasNextKey", "()Z");
  jmethodID nextKeyMethod = env->GetMethodID(readableMapKeySetIteratorClass, "nextKey", "()Ljava/lang/String;");
  jmethodID getTypeMethod =
      env->GetMethodID(readableMapClass, "getType", "(Ljava/lang/String;)Lcom/facebook/react/bridge/ReadableType;");
  jmethodID getStringMethod = env->GetMethodID(readableMapClass, "getString", "(Ljava/lang/String;)Ljava/lang/String;");
  jmethodID getDoubleMethod = env->GetMethodID(readableMapClass, "getDouble", "(Ljava/lang/String;)D");
  jmethodID getBooleanMethod = env->GetMethodID(readableMapClass, "getBoolean", "(Ljava/lang/String;)Z");
  jmethodID getMapMethod =
      env->GetMethodID(readableMapClass, "getMap", "(Ljava/lang/String;)Lcom/facebook/react/bridge/ReadableMap;");
  jmethodID getArrayMethod =
      env->GetMethodID(readableMapClass, "getArray", "(Ljava/lang/String;)Lcom/facebook/react/bridge/ReadableArray;");

  jobject keySetIterator = env->CallObjectMethod(readableMap, keySetIteratorMethod);

  while (env->CallBooleanMethod(keySetIterator, hasNextKeyMethod)) {
    // key = nextKey(readableMap)
    jobject keyObject = env->CallObjectMethod(keySetIterator, nextKeyMethod);
    const char* key = env->GetStringUTFChars((jstring)keyObject, JNI_FALSE);

    // type = getType(readableMap[key])
    jobject typeObject = env->CallObjectMethod(readableMap, getTypeMethod, keyObject);
    jclass readableTypeClass = env->FindClass("com/facebook/react/bridge/ReadableType");
    jmethodID getStringNameMethod = env->GetMethodID(readableTypeClass, "name", "()Ljava/lang/String;");
    jstring typeString = (jstring)env->CallObjectMethod(typeObject, getStringNameMethod);
    const char* type = env->GetStringUTFChars(typeString, JNI_FALSE);

    LOGI("key: %s, type: %s", key, type);

    if (strcmp(type, "String") == 0) {
      jstring value = (jstring)env->CallObjectMethod(readableMap, getStringMethod, keyObject);
      result[key] = convertJstring(env, value);
      LOGI("key: %s, value: %s", key, convertJstring(env, value).c_str());
    } else if (strcmp(type, "Number") == 0) {
      double value = env->CallDoubleMethod(readableMap, getDoubleMethod, keyObject);
      result[key] = value;
    } else if (strcmp(type, "Boolean") == 0) {
      bool value = env->CallBooleanMethod(readableMap, getBooleanMethod, keyObject);
      result[key] = value;
    } else if (strcmp(type, "Map") == 0) {
      jobject nestedMap = env->CallObjectMethod(readableMap, getMapMethod, keyObject);
      result[key] = convertReadableMap(env, nestedMap);
    } else if (strcmp(type, "Array") == 0) {
      jobject array = env->CallObjectMethod(readableMap, getArrayMethod, keyObject);
      result[key] = convertReadableArray(env, array);
    }

    env->ReleaseStringUTFChars((jstring)keyObject, key);
    env->ReleaseStringUTFChars(typeString, type);
  }

  return result;
}
