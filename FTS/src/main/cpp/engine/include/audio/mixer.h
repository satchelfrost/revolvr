#pragma once


#include <cstdint>

namespace rvr {
class Mixer {
public:
    void Render(float* audioData, int32_t numFrames);
private:
    float mixingBuffer_[1920];
};
}