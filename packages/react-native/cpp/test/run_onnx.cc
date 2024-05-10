#include "run_onnx.h"
#include <iostream>

void run_onnx(const std::string &model_path)
{

	Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");

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

	// Get input_tensors
	std::vector<float> input_values_1 = {1.0f, 2.0f, 3.0f};
	std::vector<int64_t> input_shape = {1, 3, 224, 224};
	auto memory_info =
			Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
	Ort::Value input_tensor_1 = Ort::Value::CreateTensor<float>(
			memory_info, input_values_1.data(), input_values_1.size(),
			input_shape.data(), input_shape.size());
	std::vector<Ort::Value> input_tensors;
	input_tensors.emplace_back(std::move(input_tensor_1));

	std::vector<Ort::Value> output_tensors =
			session.Run(Ort::RunOptions{nullptr}, input_names.data(),
									input_tensors.data(), 2, output_names.data(), 1);

	std::cout << std::fixed;
	for (auto j = 0; j < output_tensors.size(); j++)
	{
		const float *floatarr = output_tensors[j].GetTensorMutableData<float>();
		for (int i = 0; i < 3; i++)
		{
			std::cout << floatarr[i] << " ";
		}
		std::cout << std::endl;
	}
}