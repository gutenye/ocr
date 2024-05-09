#include "onnx_runner.h"
#include <iostream>

int main() {
    OnnxRunner model_runner("model.onnx");
    std::vector<float> input_tensor_values(1 * 3 * 224 * 224, 1.0f); // Example: adapt to your actual input format
    std::vector<int64_t> input_tensor_shape = {1, 3, 224, 224};

    std::vector<float> results = model_runner.run(input_tensor_values, input_tensor_shape);
  
    std::cout << "Model output (first 10 results): ";
    for (const auto& value : results) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    return 0;
}
