#pragma once

#include <vector>

struct ImageRaw
{
	std::vector<float> data{};
	int64_t width{};
	int64_t height{};
	int64_t channels{};
};