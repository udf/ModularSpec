#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <thread>

#include <ncurses.h>

#include "OpenALDataFetcher.h"
#include "Spectrum.h"

const size_t fft_size = 4096;
const size_t sample_rate = 44100;
const size_t num_bars = 50;

void draw_bars(float data[], uint start_x, uint start_y, uint height, size_t width) {
    for (size_t x = 0; x < width; x++) {
        size_t bar_height = round(pow(data[x], 0.2f) * height);

        for (size_t y = 0; y < height; y++) {
            mvaddch(height - y + start_y, x + start_x, y <= bar_height ? '|' : ' ');
        }
    }
}

void start_curses() {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);
}

void stop_curses() {
    endwin();
}

int main() {
    float audio_data[fft_size];
    OpenALDataFetcher audio_fetcher(
        sample_rate,
        fft_size,
        [](const std::vector<std::string> &list) {
            stop_curses();

            for (size_t i = 0; i < list.size(); ++i) {
                std::cout << i << ": " << list[i] << std::endl;
            }

            size_t device_id = 0;
            std::cout << "Enter a device number to use: ";
            std::cin >> device_id;
            std::cout << "Using device #" << device_id << std::endl << std::endl;

            start_curses();
            return device_id;
        }
    );

    Spectrum spec(fft_size);
    spec.UseLinearNormalisation(1, 20);
    float bar_data[num_bars];

    while (true) {
        int ch = getch();
        if (ch == 'q')
            break;

        audio_fetcher.UpdateData();
        audio_fetcher.GetData(audio_data);
        spec.Update(audio_data);
        spec.GetData(20, 5000, sample_rate, bar_data, num_bars);

        draw_bars(bar_data, 0, 0, 30, num_bars);
        refresh();

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    stop_curses();
    std::cout << "stop" << std::endl;
    return 0;
}
