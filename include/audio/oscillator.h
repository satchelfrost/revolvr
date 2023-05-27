#pragma once

#include <atomic>
#include <stdint.h>

class Oscillator {
public:
    void SetWaveOnAndScale(bool isWaveOn, double scale);
    void SetSampleRate(int32_t sampleRate);
    void Render(float *audioData, int32_t numFrames);

private:
    std::atomic<bool> isWaveOn_{false};
    double phase_ = 0.0;
    double phaseIncrement_ = 0.0;
    double sound_scale_ = 1.0;
};
