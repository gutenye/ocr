#pragma once

#include <onnxruntime_cxx_api.h>
#include <vector>

struct ModelOutput {
  std::vector<float> data {};
  std::vector<int64_t> shape {};
};

using AllocatedStringPtr = std::unique_ptr<char, Ort::detail::AllocatedFree>;

class Onnx {
public:
  Onnx(const std::string &model_path);
  ModelOutput run(std::vector<float> &input, const std::vector<int64_t> &input_shape);

private:
  Ort::Env m_env;
  Ort::Session m_session;
  std::vector<const char *> m_input_names {};
  std::vector<const char *> m_output_names {};
  Ort::AllocatorWithDefaultOptions m_allocator;
  std::vector<AllocatedStringPtr> m_input_name_allocated;
  std::vector<AllocatedStringPtr> m_output_name_allocated;
};
