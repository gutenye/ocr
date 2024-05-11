#include "onnx.h"
#include <format>
#include <iostream>
#include "timer.h"

Onnx::Onnx(const std::string &model_path)
    : env{ORT_LOGGING_LEVEL_WARNING, "ocr"}, m_session{env, model_path.c_str(), Ort::SessionOptions()} {
  // Get input/output node names
  size_t numInputNodes = m_session.GetInputCount();
  for (int i = 0; i < numInputNodes; i++) {
    auto input_name = m_session.GetInputNameAllocated(i, m_allocator);
    m_input_name_allocated.push_back(std::move(input_name));
    m_input_names.emplace_back(m_input_name_allocated.back().get());
  }
  size_t numOutputNodes = m_session.GetOutputCount();
  for (int i = 0; i < numOutputNodes; i++) {
    auto output_name = m_session.GetOutputNameAllocated(i, m_allocator);
    m_output_name_allocated.push_back(std::move(output_name));
    m_output_names.emplace_back(m_output_name_allocated.back().get());
  }
}

ModelOutput Onnx::run(std::vector<float> &input, const std::vector<int64_t> &input_shape) {
  // Get input
  auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
  Ort::Value iput_tensor =
      Ort::Value::CreateTensor<float>(memory_info, input.data(), input.size(), input_shape.data(), input_shape.size());
  std::vector<Ort::Value> input_tensors;
  input_tensors.emplace_back(std::move(iput_tensor));

  // Run model
  Timer tic;
  tic.start();

  std::vector<Ort::Value> output_tensors =
      m_session.Run(Ort::RunOptions{nullptr}, m_input_names.data(), input_tensors.data(), m_input_names.size(),
                    m_output_names.data(), m_output_names.size());
  tic.end();
  auto runTime = tic.get_average_ms();
  std::cout << "onnx run costs " << runTime << std::endl;

  // Return output
  auto &output_tensor = output_tensors.front();
  auto tensor_info = output_tensor.GetTensorTypeAndShapeInfo();
  auto shape = tensor_info.GetShape();

  // Convert float* to vector<float>
  auto *floatarr = output_tensor.GetTensorMutableData<float>();
  int64_t size;
  // det
  if (shape.size() == 4) {
    size = shape[0] * shape[1] * shape[2] * shape[3];
  } else
  // rec
  {
    size = shape[0] * shape[1] * shape[2];
  }
  std::vector<float> data(floatarr, floatarr + size);

  ModelOutput model_output{.data = data, .shape = shape};

  return model_output;
}