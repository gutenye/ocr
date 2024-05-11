#include "run_onnx.h"
#include <iostream>
#include <format>
#include "timer.h"

ModelOutput run_onnx(const std::string &model_path, std::vector<float> &input, const std::vector<int64_t> &input_shape)
{
	Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "ocr");
	Ort::SessionOptions session_options;
	Ort::Session session(env, model_path.c_str(), session_options);

	// Get input/output node names
	using AllocatedStringPtr = std::unique_ptr<char, Ort::detail::AllocatedFree>;
	std::vector<const char *> input_names;
	std::vector<AllocatedStringPtr> inputNodeNameAllocatedStrings;
	std::vector<const char *> output_names;
	std::vector<AllocatedStringPtr> outputNodeNameAllocatedStrings;
	Ort::AllocatorWithDefaultOptions allocator;
	size_t numInputNodes = session.GetInputCount();
	for (int i = 0; i < numInputNodes; i++)
	{
		auto input_name = session.GetInputNameAllocated(i, allocator);
		inputNodeNameAllocatedStrings.push_back(std::move(input_name));
		input_names.emplace_back(inputNodeNameAllocatedStrings.back().get());
	}
	size_t numOutputNodes = session.GetOutputCount();
	for (int i = 0; i < numOutputNodes; i++)
	{
		auto output_name = session.GetOutputNameAllocated(i, allocator);
		outputNodeNameAllocatedStrings.push_back(std::move(output_name));
		output_names.emplace_back(outputNodeNameAllocatedStrings.back().get());
	}

	// Get input
	auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
	Ort::Value iput_tensor = Ort::Value::CreateTensor<float>(memory_info, input.data(), input.size(), input_shape.data(), input_shape.size());
	std::vector<Ort::Value> input_tensors;
	input_tensors.emplace_back(std::move(iput_tensor));

	// Run model
	Timer tic;
	tic.start();
	std::vector<Ort::Value> output_tensors =
			session.Run(Ort::RunOptions{nullptr}, input_names.data(),
									input_tensors.data(), input_names.size(), output_names.data(), output_names.size());
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
	if (shape.size() == 4)
	{
		size = shape[0] * shape[1] * shape[2] * shape[3];
	}
	else
	// rec
	{
		size = shape[0] * shape[1] * shape[2];
	}
	std::vector<float> data(floatarr, floatarr + size);

	ModelOutput model_output{.data = data, .shape = shape};

	return model_output;
}