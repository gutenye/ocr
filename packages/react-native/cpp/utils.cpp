// Copyright (c) 2021 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "utils.h"
#include <iostream>
#include <vector>

void NHWC3ToNC3HW(const float *din, float *dout, int size, const std::vector<float> mean,
                  const std::vector<float> scale) {
    if (mean.size() != 3 || scale.size() != 3) {
        std::cerr << "[ERROR] mean or scale size must equal to 3\n" << std::endl;
        return;
    }

    float *dout_c0 = dout;
    float *dout_c1 = dout + size;
    float *dout_c2 = dout + size * 2;

    for (int i = 0; i < size; i++) {
        dout_c0[i] = (din[i * 3 + 0] - mean[0]) * scale[0];
        dout_c1[i] = (din[i * 3 + 1] - mean[1]) * scale[1];
        dout_c2[i] = (din[i * 3 + 2] - mean[2]) * scale[2];
    }
}

void NHWC1ToNC1HW(const float *src, float *dst, const float *mean, const float *std, int width, int height) {
    int size = height * width;
    float vmean = (mean ? mean[0] : 0.0f);
    float vscale = (std ? (1.0f / std[0]) : 1.0f);

    for (int i = 0; i < size; i++) {
        dst[i] = (src[i] - vmean) * vscale;
    }
}
