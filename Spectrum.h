#define _USE_MATH_DEFINES

#include <stddef.h>
#include <math.h>
#include <iostream>
#include <algorithm>

#include <fftw3.h>

class Spectrum
{
public:
    Spectrum(const size_t size);
    ~Spectrum();
    void UseHanningWindow();
    void UseHammingWindow();
    void UseBlackmanWindow();
    void Update(const float data[]);

    // configurables
    float scale = 2.0f;
    float average_weight = 0.9f;
    float *window;

    // output
    float *bar_data;

private:
    size_t size, bar_data_size;

    double *fft_in;
    fftw_complex *fft_out;
    fftw_plan plan;
};