#define _USE_MATH_DEFINES

#include <stddef.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <memory>

#include <fftw3.h>

#include "util.h"

class Spectrum {
public:
    Spectrum(const size_t size);
    ~Spectrum();
    void UseHanningWindow();
    void UseHammingWindow();
    void UseBlackmanWindow();
    void Update(const float data[]);
    float BarDataAt(float i) const;
    void GetData(
        float freq_min,
        float freq_max,
        float sample_rate,
        float output[],
        size_t output_size
    ) const;

    // configurables
    float scale = 1;
    float average_weight = 0.6;
    std::unique_ptr<float[]> window;

    // output
    std::unique_ptr<float[]> bar_data;

private:
    size_t size, bar_data_size;

    std::unique_ptr<double[]> fft_in;
    fftw_complex *fft_out;
    fftw_plan plan;
};