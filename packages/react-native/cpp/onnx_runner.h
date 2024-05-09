#include <onnxruntime_cxx_api.h>
#include <vector>
#include <string>

class OnnxRunner {
public:
    explicit OnnxRunner(const std::string& model_path);
    ~OnnxRunner();
    std::vector<float> run(const std::vector<float>& input_tensor_values, const std::vector<int64_t>& input_tensor_shape);

private:
    void setupInputOutputNames();

    Ort::Env env;
    Ort::Session session;
    std::vector<const char*> input_node_names;
    std::vector<const char*> output_node_names;
    Ort::AllocatorWithDefaultOptions allocator;
};