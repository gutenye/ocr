#pragma once

#include <iostream>
#include <vector>

struct ImageRaw {
  std::vector<float> data {};
  int64_t width {};
  int64_t height {};
  int64_t channels {};
};

struct ModelPerformance {
  float total_time {};
  float preprocess_time {};
  float predict_time {};
  float postprocess_time {};
};

struct Frame {
  int top {};
  int left {};
  int width {};
  int height {};
};

std::ostream& operator<<(std::ostream& os, const Frame& frame);
