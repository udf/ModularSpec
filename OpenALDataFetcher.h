#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>
#include <stddef.h>

#include <AL/al.h>
#include <AL/alc.h>

class OpenALDataFetcher
{
public:
    OpenALDataFetcher();
    ~OpenALDataFetcher();
    
    void SetInternalBufferSize(const size_t size);
    bool GetData(float buffer[], const size_t length);
    bool UseDevice(const size_t device_id);
    void PrintDeviceList();
    const std::vector<std::string>& GetDeviceList();
    void BuildDeviceList();

private:
    std::vector<std::string> device_names;
    ALCdevice *device = NULL;
    size_t device_id;
    size_t internal_buffer_size = 2048;
    unsigned char *internal_buffer = NULL;
};