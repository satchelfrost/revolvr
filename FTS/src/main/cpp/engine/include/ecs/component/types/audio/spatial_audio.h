#pragma once

#include <ecs/component/types/audio.h>
#include <audio/circular_buffer.h>

namespace rvr {
class SpatialAudio : public Audio {
public:
    SpatialAudio(type::EntityId pId, WavAudioSource* wavAudioSource);
    virtual void Render(float* targetData, int32_t numSamples) override;
    virtual void Play() override;
    static void ResetHead();

private:
    void Spatialize();

    CircularBuffer circularBuffer_{8192};
    int32_t totalSourceFrames_;
    const float* data_;
};
}
