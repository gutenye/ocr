#pragma once

#include <vector>
#include <onnxruntime_cxx_api.h>

struct ModelOutput
{
	std::vector<float> data{};
	std::vector<int64_t> shape{};
};

class Onnx
{
public:
	Onnx(const std::string &model_path);
	ModelOutput run(std::vector<float> &input, const std::vector<int64_t> &input_shape);

private:
	Ort::Env env; // Declare environment as a member if needed for the lifetime of Onnx class
	Ort::SessionOptions session_options;
	Ort::Session m_session;
	std::vector<const char *> m_input_names{};
	std::vector<const char *> m_output_names{};
	Ort::AllocatorWithDefaultOptions m_allocator;
	using AllocatedStringPtr = std::unique_ptr<char, Ort::detail::AllocatedFree>;
	std::vector<AllocatedStringPtr> m_input_name_allocated;
	std::vector<AllocatedStringPtr> m_output_name_allocated;
};
