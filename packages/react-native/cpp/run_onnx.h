#pragma once

#include <onnxruntime_cxx_api.h>

struct ModelOutput
{
	std::vector<float> data{};
	std::vector<int64_t> shape{};
};

ModelOutput run_onnx(const std::string &model_path, std::vector<float> &input, std::vector<int64_t> &input_shape);