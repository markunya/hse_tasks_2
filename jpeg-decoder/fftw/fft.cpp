#include <fft.h>

#include <fftw3.h>
#include <math.h>

class DctCalculator::Impl {
public:
    Impl(size_t width, std::vector<double>* input, std::vector<double>* output)
        : width_(width), input_(input), output_(output) {
    }

    void Inverse() {
        for (size_t i = 0; i < width_; ++i) {
            for (size_t j = 0; j < width_; ++j) {
                size_t real_index = i * width_ + j;
                double factor = 1.0;
                if (i > 0 && j > 0) {
                    factor = 2.0;
                } else if (i > 0 || j > 0) {
                    factor = sqrt(2);
                }
                input_->at(real_index) /= static_cast<double>(width_) * factor;
            }
        }
        fftw_plan plan = fftw_plan_r2r_2d(width_, width_, input_->data(), output_->data(),
                                          FFTW_REDFT01, FFTW_REDFT01, FFTW_ESTIMATE);
        fftw_execute(plan);
        fftw_destroy_plan(plan);
        fftw_cleanup();
    }

private:
    size_t width_;
    std::vector<double>* input_;
    std::vector<double>* output_;
};

DctCalculator::DctCalculator(size_t width, std::vector<double>* input,
                             std::vector<double>* output) {
    if (input->size() != output->size()) {
        throw std::invalid_argument("Input and output sizes should be equal");
    }
    if (input->size() != width * width) {
        throw std::invalid_argument("Width * width != input size");
    }
    impl_ = std::make_unique<Impl>(width, input, output);
}

void DctCalculator::Inverse() {
    impl_->Inverse();
}

DctCalculator::~DctCalculator() = default;
