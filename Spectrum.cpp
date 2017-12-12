#include "Spectrum.h"

Spectrum::Spectrum(const size_t size) {
    this->size = size;
    bar_data_size = size/2;

    bar_data = new float[bar_data_size]();
    window = new float[size];
    UseHanningWindow();

    fft_in = new double[size];
    fft_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size);
    plan = fftw_plan_dft_r2c_1d(size, fft_in, fft_out, FFTW_ESTIMATE);
}

Spectrum::~Spectrum() {
    fftw_destroy_plan(plan);
    fftw_free(fft_out);
    delete fft_in;
    delete window;
    delete bar_data;
}

void Spectrum::UseHanningWindow() {
    double phase = 0;
    double delta = 2 * M_PI / (double)size;

    for (size_t i = 0; i < size; i++) {
        window[i] = (float)(0.5 * (1.0 - cos(phase)));
        phase += delta;
    }
}

void Spectrum::UseHammingWindow() {
    double phase = 0;
    double delta = 2 * M_PI / (double)size;

    for (size_t i = 0; i < size; i++) {
        window[i] = (float)(0.54 - .46*cos(phase));
        phase += delta;
    }
}

void Spectrum::UseBlackmanWindow() {
    double phase = 0;
    double delta = 2 * M_PI / (double)size;

    for (size_t i = 0; i < size; i++) {
        window[i] = (float)(0.42 - .5*cos(phase) + .08*cos(2 * phase));
        phase += delta;
    }
}

void Spectrum::Update(const float data[]) {
    // apply window and scale to data and copy into input array
    for (size_t i = 0; i < size; i++) {
        fft_in[i] = data[i] * window[i] * scale;
    }

    fftw_execute(plan);

    // convert data from output to usable data
    for (size_t i = 0; i < size / 2; i++) {
        bar_data[i] = bar_data[i] * (1 - average_weight);

        // Compute log magnitude from real and imaginary components of FFT 
        // 0.5 * log(x) == log(sqrt(x))
        float mag = 0.5f * log10(fft_out[i][0]*fft_out[i][0] + fft_out[i][1]*fft_out[i][1]);

        // Clip magnitude to [0, 1]
        mag = std::max(0.0f, std::min(1.0f, mag));

        bar_data[i] += mag * average_weight;
    }
}