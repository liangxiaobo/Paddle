/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserve.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include "paddle/framework/device_data_transform.h"

namespace paddle {
namespace framework {

static const platform::DeviceContext* GetDeviceContext(
    const platform::Place& src_place, const platform::Place& dst_place) {
  platform::DeviceContextPool& pool = platform::DeviceContextPool::Instance();

  if (platform::is_gpu_place(src_place) && platform::is_cpu_place(dst_place)) {
    return pool.Get(src_place);
  } else if (platform::is_cpu_place(src_place) &&
             platform::is_gpu_place(dst_place)) {
    return pool.Get(dst_place);
  } else {
    PADDLE_THROW(
        "Currently, model parallelism is only supported between CPU and CUDA");
  }
}

Tensor* DeviceTransform(const Tensor& in, const platform::Place& dst_place) {
  VLOG(3) << "DeviceTransform in, src_place " << in.place()
          << " dst_place: " << dst_place;
  Tensor* out = new Tensor();
  auto* dev_ctx = GetDeviceContext(in.place(), dst_place);
  dev_ctx->Wait();
  Copy(in, dst_place, *dev_ctx, out);
  dev_ctx->Wait();
  return out;
}

}  // namespace framework
}  // namespace paddle
