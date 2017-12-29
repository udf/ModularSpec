#include "OpenALDataFetcher.h"

OpenALDataFetcher::OpenALDataFetcher(
    const ALCuint sample_rate,
    const ALCsizei buffer_size,
    const std::function<size_t(const std::vector<std::string>&)> device_matcher
) {
    internal_buffer = std::make_unique<short[]>(buffer_size);
    internal_buffer_size = buffer_size;
    this->sample_rate = sample_rate;
    this->device_matcher = device_matcher;

    ReloadDevice();
}

OpenALDataFetcher::~OpenALDataFetcher() {
    if (device)
        alcCaptureCloseDevice(device);
}

void OpenALDataFetcher::ReloadDevice() {
    if (device)
        alcCaptureCloseDevice(device);

    // refresh the device list
    BuildDeviceList();
    if (device_list.size() == 0)
        throw std::runtime_error("Could not find any capture devices");

    // call the matcher and check its output
    size_t device_id = device_matcher(device_list);
    if (device_id >= device_list.size()) {
        std::cerr << "matcher chose out of bounds device_id (" << device_id << ")";
        std::cerr << " defaulting to 0 (" << device_list[0] << ")" << std::endl;
        device_id = 0;
    }

    // try to open the device chosen by the matcher
    device = alcCaptureOpenDevice(
        device_list[device_id].c_str(),
        sample_rate,
        AL_FORMAT_MONO16,
        internal_buffer_size
    );
    if (device == NULL)
        throw std::runtime_error("Could not open device");

    // try to start capturing from the device
    alcCaptureStart(device);
    if(alcGetError(device) != ALC_NO_ERROR)
        throw std::runtime_error("Could not start capture");
}

void OpenALDataFetcher::BuildDeviceList() {
    device_list.clear();
    const ALCchar* devices = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
    if (*devices == '\0')
        return;

    do {
        device_list.push_back(std::string(devices));
        devices += device_list.back().size() + 1;
    } while (*devices != '\0');
}

ALCint OpenALDataFetcher::UpdateData() {
    ALCint available_samples;
    alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, 1, &available_samples);

    ALCsizei readable_samples = std::min(available_samples, internal_buffer_size);
    if (available_samples > 0) {
        for (ALCsizei i = readable_samples; i < internal_buffer_size; ++i) {
            internal_buffer[i - readable_samples] = internal_buffer[i];
        }

        alcCaptureSamples(
            device,
            (ALCvoid*)(internal_buffer.get() + internal_buffer_size - readable_samples),
            readable_samples
        );
    }

    return readable_samples;
}

void OpenALDataFetcher::GetData(float buffer[]) {
    for (ALCsizei i = 0; i < internal_buffer_size; i++) {
        buffer[i] = map(internal_buffer[i], -32768, 32767, -1, 1);
    }
}