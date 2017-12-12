#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <math.h>

#include "OpenALDataFetcher.h"
#include "Spectrum.h"

#define mssleep(ms) usleep(ms * 1000)
#define FFT_SIZE 1024
#define N_BARS (FFT_SIZE / 2)
#define N_BANDS 50

float map(float n, float min1, float max1, float min2, float max2) {
    return min2 + (max2 - min2) * ((n - min1) / (max1 - min1));
}

int main() {
    using std::cout;
    using std::endl;

    OpenALDataFetcher audio_fetcher;
    audio_fetcher.PrintDeviceList();
    // TODO make this selectable
    if (!audio_fetcher.UseDevice(3)) {
        std::cerr << "Could not open device" << endl;
        return 1;
    }
    Spectrum spec(FFT_SIZE);

    float audio_data[FFT_SIZE] = {0};

    cout << "\33[?25l" << endl;
    float bands[N_BANDS] = {0};
    const float scale = (FFT_SIZE / 2) / (float)(N_BANDS);

    // TODO add quit listen
    // and keys for altering parameters
    while (true) {
        audio_fetcher.GetData(audio_data, FFT_SIZE);
        spec.Update(audio_data);

        // TODO make function that does slicing based on a frequency range
        for (size_t i = 0; i < N_BANDS; i++) {
            bands[i] = 0;
        }
        for (size_t i = 0; i < N_BARS; i++) {
            bands[(size_t)round(map(i, 0, N_BARS, 0, N_BANDS))] += spec.bar_data[i];
        }
        for (size_t i = 0; i < N_BANDS; i++) {
            bands[i] /= scale;
        }

        cout << "\r";
        for (size_t i = 0; i < N_BANDS; i++) {
            cout << "\33[48;5;" << (int)(bands[i] * 23) + 232 << "m ";
        }
        cout << std::flush;

        mssleep(20);
    }
    cout << "\33[0m" << endl;

    return 0;
}