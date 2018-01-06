#include <vector>
#include <string>
#include <unistd.h>
#include <stddef.h>
#include <functional>
#include <memory>
#include <stdexcept>
#include <ctime>

#include <AL/al.h>
#include <AL/alc.h>

#include "util.h"

class OpenALDataFetcher {
public:
    OpenALDataFetcher(
        ALCuint sample_rate,
        ALCsizei buffer_size,
        const std::function<size_t(const std::vector<std::string>&)> &device_matcher
    );
    ~OpenALDataFetcher();

    ALCint UpdateData();
    void GetData(float buffer[]);
    void ReloadDevice();

    double device_timeout = 0.1;

private:
    void BuildDeviceList();

    clock_t last_capture;

    ALCuint sample_rate;
    ALCsizei internal_buffer_size;
    std::function<size_t(const std::vector<std::string> &device_list)> device_matcher;

    ALCdevice *device = NULL;
    std::unique_ptr<short[]> internal_buffer;
    std::vector<std::string> device_list;
};