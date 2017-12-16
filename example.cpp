#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ncurses.h>

#include "OpenALDataFetcher.h"
#include "Spectrum.h"

#define mssleep(ms) usleep(ms * 1000)
#define FFT_SIZE 1024
#define SAMPLE_RATE 25000
#define N_BARS (FFT_SIZE / 2)
#define N_BANDS 50

float map(float n, float min1, float max1, float min2, float max2) {
    return min2 + (max2 - min2) * ((n - min1) / (max1 - min1));
}

void draw_bars(WINDOW *win, float data[], uint start_x, uint start_y, uint height, size_t width) {
    for (size_t x = 0; x < width; x++) {
        size_t bar_height = round(data[x]*height);

        for (size_t y = 0; y < height; y++) {
            mvwaddch(win, height - y, x, y <= bar_height ? 'A' : ' ');
        }
    }
}

int main() {
    // Open audio device
    OpenALDataFetcher audio_fetcher;
    audio_fetcher.PrintDeviceList();

    size_t device_id = 0;
    std::cout << "Enter a device number to use: ";
    std::cin >> device_id;
    std::cout << "Using device #" << device_id << std::endl;

    if (!audio_fetcher.UseDevice(device_id, SAMPLE_RATE)) {
        std::cerr << "Could not open device" << std::endl;
        return 1;
    }


    float audio_data[FFT_SIZE] = {0};
    Spectrum spec(FFT_SIZE);
    spec.average_weight = 0.8f;

    float bands[N_BANDS] = {0};
    const float scale = (FFT_SIZE / 2) / (float)(N_BANDS);

    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);

    WINDOW *win = newwin(25, N_BANDS, 0, 0);

    while (true) {
        int ch = getch();
        if (ch == 'q') {
            break;
        }

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

        draw_bars(win, bands, 0, 0, 25, N_BANDS);
        wrefresh(win);

        mssleep(20);
    }

    delwin(win);
    endwin();
    return 0;
}