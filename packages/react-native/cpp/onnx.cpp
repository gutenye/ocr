#include "onnx.h"
#include <iostream>
#include "timer.h"

Onnx::Onnx(const std::string &model_path)
    : m_env(ORT_LOGGING_LEVEL_WARNING, "ocr"), m_session {m_env, model_path.c_str(), Ort::SessionOptions()} {
  // Get input / output node names
  size_t input_nodes_num = m_session.GetInputCount();
  for (int i = 0; i < input_nodes_num; i++) {
    auto input_name = m_session.GetInputNameAllocated(i, m_allocator);
    m_input_name_allocated.push_back(std::move(input_name));
    m_input_names.emplace_back(m_input_name_allocated.back().get());
  }
  size_t output_nodes_num = m_session.GetOutputCount();
  for (int i = 0; i < output_nodes_num; i++) {
    auto output_name = m_session.GetOutputNameAllocated(i, m_allocator);
    m_output_name_allocated.push_back(std::move(output_name));
    m_output_names.emplace_back(m_output_name_allocated.back().get());
  }
}

ModelOutput Onnx::run(std::vector<float> &input, const std::vector<int64_t> &input_shape) {
  // Get input
  auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
  Ort::Value input_tensor =
      Ort::Value::CreateTensor<float>(memory_info, input.data(), input.size(), input_shape.data(), input_shape.size());
  std::vector<Ort::Value> input_tensors;
  input_tensors.emplace_back(std::move(input_tensor));

  // Run model
  std::vector<Ort::Value> output_tensors =
      m_session.Run(Ort::RunOptions {nullptr}, m_input_names.data(), input_tensors.data(), m_input_names.size(),
                    m_output_names.data(), m_output_names.size());

  // Return output
  auto &output_tensor = output_tensors.front();
  auto tensor_info = output_tensor.GetTensorTypeAndShapeInfo();
  auto shape = tensor_info.GetShape();

  // Convert float* to vector<float>
  auto *float_array = output_tensor.GetTensorMutableData<float>();
  int64_t size;
  // det
  if (shape.size() == 4) {
    size = shape[0] * shape[1] * shape[2] * shape[3];
  } else
  // rec
  {
    size = shape[0] * shape[1] * shape[2];
  }
  std::vector<float> data(float_array, float_array + size);

  ModelOutput model_output {.data = data, .shape = shape};

  return model_output;
}