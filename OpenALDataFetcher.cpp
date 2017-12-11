#include "OpenALDataFetcher.h"

OpenALDataFetcher::OpenALDataFetcher() {
    BuildDeviceList();
    device_id = 0;
    SetInternalBufferSize(internal_buffer_size);
}

OpenALDataFetcher::~OpenALDataFetcher() {
    if (device) {
        alcCaptureCloseDevice(device);
    }
    if (internal_buffer) {
        delete internal_buffer;
    }
}

void OpenALDataFetcher::SetInternalBufferSize(size_t size) {
    if (internal_buffer)
        delete internal_buffer;

    internal_buffer_size = size;
    internal_buffer = new unsigned char[internal_buffer_size];
    UseDevice(device_id);
}

bool OpenALDataFetcher::GetData(float buffer[], size_t length) {
    if (!device)
        return false;
    if (length > internal_buffer_size) 
        SetInternalBufferSize(length);

    // block until there is enough data
    ALCint available_samples;
    do {
        alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, 1, &available_samples);
        usleep(1000);
    } while (available_samples < length);

    alcCaptureSamples(device, (ALCvoid*)internal_buffer, length);
    for (size_t i = 0; i < length; ++i) {
        buffer[i] = (internal_buffer[i] - 128) / 128.0f;
    }
    return true;
}

bool OpenALDataFetcher::UseDevice(size_t device_id) {
    if (device_id >= device_names.size())
        return false;

    if (device)
        alcCaptureCloseDevice(device);

    device = alcCaptureOpenDevice(device_names[device_id].c_str(), 10000, AL_FORMAT_MONO8, internal_buffer_size);
    if (!device)
        return false;
    alcCaptureStart(device);
    this->device_id = device_id;

    return true;
}

void OpenALDataFetcher::PrintDeviceList() {
    for (size_t i = 0; i < device_names.size(); i++) {
        std::cout << "[" << i << "] " << device_names[i] << std::endl;
    }
}

const std::vector<std::string>& OpenALDataFetcher::GetDeviceList() {
    return device_names;
}

void OpenALDataFetcher::BuildDeviceList() {
    device_names.clear();
    const ALCchar* devices = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);

    do {
        device_names.push_back(std::string(devices));
        devices += device_names.back().size() + 1;
    } while (*devices != '\0');
}
