#include <iostream>
#include <unistd.h>

#include "OpenALDataFetcher.h"

#define mssleep(ms) usleep(ms * 1000)

int main() {
    OpenALDataFetcher test = OpenALDataFetcher();
    test.PrintDeviceList();
    if (!test.UseDevice(3)) {
        std::cerr << "Could not open device" << std::endl;
        return 1;
    }

    float data[256] = {0};
    std::cout.precision(2);

    while (true) {
        test.GetData(data, 256);

        for (size_t i = 0; i < 256; i++) {
            std::cout << data[i] << " ";
        }
        std::cout << std::endl;

        mssleep(100);
    }

    return 0;
}