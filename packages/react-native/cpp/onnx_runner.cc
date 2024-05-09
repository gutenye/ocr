#include "onnx_runner.h"
#include <iostream>

OnnxRunner::OnnxRunner(const std::string& model_path) : env(ORT_LOGGING_LEVEL_WARNING, "test") {
    Ort::SessionOptions session_options;
    session_options.SetIntraOpNumThreads(1);
    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
    session = Ort::Session(env, model_path.c_str(), session_options);
    
    setupInputOutputNames();
}

void OnnxRunner::setupInputOutputNames() {
    input_node_names.push_back(session.GetInputName(0, allocator));
    output_node_names.push_back(session.GetOutputName(0, allocator));
}

std::vector<float> OnnxRunner::run(const std::vector<float>& input_tensor_values, const std::vector<int64_t>& input_tensor_shape) {
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(allocator, input_tensor_values.data(), 
        input_tensor_values.size(), input_tensor_shape.data(), input_tensor_shape.size());
    auto output_tensors = session.Run(Ort::RunOptions{nullptr}, input_node_names.data(),
        &input_tensor, 1, output_node_names.data(), 1);
    float* floatarr = output_tensors.front().GetTensorMutableData<float>();

    return std::vector<float>(floatarr, floatarr + 10); // Assuming the output size is known
}

OnnxRunner::~OnnxRunner() {
    for (auto* name : input_node_names) {
        allocator.Free(name);
    }
    for (auto* name : output_node_names) {
        allocator.Free(name);
    }
}
