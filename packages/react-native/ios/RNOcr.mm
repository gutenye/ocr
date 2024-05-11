#import "RNOcr.h"
#import <onnxruntime_cxx_api.h>
// #import <paddle_api.h>
// #import <paddle_use_kernels.h>
// #import <paddle_use_ops.h>
#import <iostream>
#import <opencv2/opencv.hpp>
#import <string>
#import <vector>
#import "native-ocr.h"

void onnx_run(const std::string &model_path);

@implementation RNOcr
RCT_EXPORT_MODULE()

RCT_EXPORT_METHOD(ocr
                  : (NSString *)img_path1 resolve
                  : (RCTPromiseResolveBlock)resolve reject
                  : (RCTPromiseRejectBlock)reject) {
  NSString *path = [[NSBundle mainBundle] bundlePath];
  std::string paddle_dir = std::string([path UTF8String]);
  // std::string det_model_file =
  //     paddle_dir + "/resources.bundle/ch_ppocr_mobile_v2.0_det_slim_opt.nb";
  // std::string rec_model_file =
  //     paddle_dir + "/resources.bundle/ch_ppocr_mobile_v2.0_rec_slim_opt.nb";
  // std::string cls_model_file =
  //     paddle_dir + "/resources.bundle/ch_ppocr_mobile_v2.0_cls_slim_opt.nb";
  std::string det_model_file = paddle_dir + "/resources.bundle/ch_PP-OCRv4_det_infer.onnx";
  std::string rec_model_file = paddle_dir + "/resources.bundle/ch_PP-OCRv4_rec_infer.onnx";
  std::string cls_model_file = paddle_dir + "/resources.bundle/ch_ppocr_mobile_v2.0_cls_infer.onnx";
  std::string img_path = paddle_dir + "/resources.bundle/cn-01.jpg";
  std::string output_img_path = paddle_dir + "/test_result.jpg";
  std::string dict_path = paddle_dir + "/resources.bundle/ppocr_keys_v1.txt";
  std::string config_path = paddle_dir + "/resources.bundle/config.txt";

  onnx_run(rec_model_file);

  // NativeOcr *pipe_ =
  //     new NativeOcr(det_model_file, cls_model_file, rec_model_file, "LITE_POWER_HIGH", 1, config_path, dict_path);
  // std::vector<std::string> res_txt;
  // pipe_->Process(img_path, output_img_path, res_txt);

  // std::ostringstream result;
  // for (int i = 0; i < res_txt.size() / 2; i++) {
  //   //   result << i << "\t" << res_txt[2*i] << "\t" << res_txt[2*i + 1] <<
  //   //   "\t";
  //   result << res_txt[2 * i] << "\n";
  // }
  // NSString *text = [NSString stringWithUTF8String:result.str().c_str()];
  // NSLog(@"%@", text);

  // resolve(text);
}

// Don't compile this code when we build for the old architecture.
#ifdef RCT_NEW_ARCH_ENABLED
- (std::shared_ptr<facebook::react::TurboModule>)getTurboModule:
    (const facebook::react::ObjCTurboModule::InitParams &)params {
  return std::make_shared<facebook::react::NativeOcrSpecJSI>(params);
}
#endif

@end

void onnx_run(const std::string &model_path) {
  printf("1\n");
  Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");
  printf("2\n");

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
  for (int i = 0; i < numInputNodes; i++) {
    auto input_name = session.GetInputNameAllocated(i, allocator);
    inputNodeNameAllocatedStrings.push_back(std::move(input_name));
    input_names.emplace_back(inputNodeNameAllocatedStrings.back().get());
  }
  size_t numOutputNodes = session.GetOutputCount();
  for (int i = 0; i < numOutputNodes; i++) {
    auto output_name = session.GetOutputNameAllocated(i, allocator);
    outputNodeNameAllocatedStrings.push_back(std::move(output_name));
    output_names.emplace_back(outputNodeNameAllocatedStrings.back().get());
  }

  // Get input_tensors
  std::vector<float> input_values_1 = {1.0f, 2.0f, 3.0f};
  std::vector<int64_t> input_shape = {1, 3, 224, 224};
  auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
  Ort::Value input_tensor_1 = Ort::Value::CreateTensor<float>(memory_info, input_values_1.data(), input_values_1.size(),
                                                              input_shape.data(), input_shape.size());
  std::vector<Ort::Value> input_tensors;
  input_tensors.emplace_back(std::move(input_tensor_1));

  std::vector<Ort::Value> output_tensors =
      session.Run(Ort::RunOptions{nullptr}, input_names.data(), input_tensors.data(), 2, output_names.data(), 1);

  std::cout << std::fixed;
  for (auto j = 0; j < output_tensors.size(); j++) {
    const float *floatarr = output_tensors[j].GetTensorMutableData<float>();
    for (int i = 0; i < 3; i++) {
      std::cout << floatarr[i] << " ";
    }
    std::cout << std::endl;
  }
}