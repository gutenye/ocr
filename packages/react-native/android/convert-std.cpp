#include "convert-std.h"

using namespace facebook::jsi;
using namespace std;

String convertStdString(Runtime& runtime, const string& stdString) {return String::createFromUtf8(runtime, stdString);}

Array convertStdVector(Runtime& runtime, const vector<string>& stdVector) {
  Array jsiArray(runtime, stdVector.size());
  for (size_t i = 0; i < stdVector.size(); ++i) {
    auto jsiString = convertStdString(runtime, stdVector[i]);
    jsiArray.setValueAtIndex(runtime, i, jsiString);
  }
  return jsiArray;
}

Array convertStdVectorTextLine(Runtime& runtime, const vector<TextLine>& textLines) {
    // Create a new JSI Array with the capacity of textLines.size()
    Array array(runtime, textLines.size());

    // Iterate through each TextLine and create a JSI object to represent it
    for (size_t i = 0; i < textLines.size(); ++i) {
        const TextLine &textLine = textLines[i];

        // Create frame object
        Object frameDict(runtime);
        frameDict.setProperty(runtime, "top", Value(textLine.frame.top));
        frameDict.setProperty(runtime, "left", Value(textLine.frame.left));
        frameDict.setProperty(runtime, "width", Value(textLine.frame.width));
        frameDict.setProperty(runtime, "height", Value(textLine.frame.height));

        // Create the main TextLine object
        Object textLineDict(runtime);
        textLineDict.setProperty(runtime, "text", convertStdString(runtime, textLine.text));
        textLineDict.setProperty(runtime, "score", Value(textLine.score));
        textLineDict.setProperty(runtime, "frame", frameDict);

        // Add textLineDict to the array
        array.setValueAtIndex(runtime, i, textLineDict);
    }

    return array;
}