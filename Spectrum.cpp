#include "Spectrum.h"

Spectrum::Spectrum(const size_t size) {
    this->size = size;
    bar_data_size = size/2;

    bar_data = std::make_unique<float[]>(bar_data_size);
    window = std::make_unique<float[]>(size);
    UseHanningWindow();

    fft_in = std::make_unique<double[]>(size);
    fft_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size);
    plan = fftw_plan_dft_r2c_1d(size, fft_in.get(), fft_out, FFTW_ESTIMATE);
}

Spectrum::~Spectrum() {
    fftw_destroy_plan(plan);
    fftw_free(fft_out);
}

void Spectrum::UseHanningWindow() {
    double delta = 2 * M_PI / (double)size;

    for (size_t i = 0; i < size; i++) {
        double phase = i * delta;
        window[i] = (float)(0.5 * (1.0 - cos(phase)));
    }
}

void Spectrum::UseHammingWindow() {
    double delta = 2 * M_PI / (double)size;

    for (size_t i = 0; i < size; i++) {
        double phase = i * delta;
        window[i] = (float)(0.54 - .46*cos(phase));
    }
}

void Spectrum::UseBlackmanWindow() {
    double delta = 2 * M_PI / (double)size;

    for (size_t i = 0; i < size; i++) {
        double phase = i * delta;
        window[i] = (float)(0.42 - .5*cos(phase) + .08*cos(2 * phase));
    }
}

void Spectrum::Update(const float data[]) {
    // apply window and scale to data and copy into input array
    for (size_t i = 0; i < size; i++) {
        fft_in[i] = data[i] * window[i] * scale;
    }

    fftw_execute(plan);

    // convert data from output to usable data
    for (size_t i = 0; i < bar_data_size; i++) {
        // Compute log magnitude from real and imaginary components of FFT 
        // 0.5 * log(x) == log(sqrt(x))
        float mag = 0.5f * log10(fft_out[i][0]*fft_out[i][0] + fft_out[i][1]*fft_out[i][1]);

        // Clip magnitude to [0, 1]
        mag = std::clamp(mag, 0.0f, 1.0f);

        // Use weighted average to smooth the data
        bar_data[i] = bar_data[i] * (1 - average_weight) + mag * average_weight;
    }
}

float Spectrum::BarDataAt(float i) const {
    if (i < 0 || i >= bar_data_size)
        return 0;

    float intpart;
    float weight = modf(i, &intpart);

    size_t start = intpart;
    size_t end = std::min(start + 1, bar_data_size - 1);

    return (1 - weight) * bar_data[start] + weight * bar_data[end];
}

void Spectrum::GetData(
    float freq_start,
    float freq_end,
    float sample_rate,
    float output[],
    size_t output_size
) const {
    const float freq_max = sample_rate/2;
    const float input_size = map(std::abs(freq_start - freq_end), 0, freq_max, 0, bar_data_size - 1);
    int neighbours = floor(input_size / output_size);

    for (size_t i = 0; i < output_size; i++) {
        const float freq = map(i, 0, output_size - 1, freq_start, freq_end);
        const float base_i = map(freq, 0, freq_max, 0, bar_data_size - 1);

        size_t count = 0;
        float out_sum = 0;
        for (int offset = -neighbours; offset <= neighbours; offset++) {
            const float input_i = base_i + offset;
            if (input_i < 0 || input_i >= bar_data_size)
                continue;

            out_sum += BarDataAt(input_i);
            count++;
        }

        output[i] = out_sum / count;
    }
}