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

#define FFT_SIZE 4096
#define SAMPLE_RATE 44100
#define BARS 50

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
    float audio_data[FFT_SIZE];
    OpenALDataFetcher audio_fetcher(
        SAMPLE_RATE,
        FFT_SIZE,
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

    Spectrum spec(FFT_SIZE);
    spec.UseLinearNormalisation(1, 20);
    float bar_data[BARS];

    while (true) {
        int ch = getch();
        if (ch == 'q')
            break;

        audio_fetcher.UpdateData();
        audio_fetcher.GetData(audio_data);
        spec.Update(audio_data);
        spec.GetData(20, 5000, SAMPLE_RATE, bar_data, BARS);

        draw_bars(bar_data, 0, 0, 30, BARS);
        refresh();

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    stop_curses();
    std::cout << "stop" << std::endl;
    return 0;
}
