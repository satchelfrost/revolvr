#pragma once

#include <ecs/component/component.h>
#include <audio/wav_audio_source.h>
#include "audio/circular_buffer.h"

namespace rvr {
class Audio : public Component {
public:
    Audio(type::EntityId pId, WavAudioSource* wavAudioSource);
    virtual Component* Clone(type::EntityId newEntityId) override;
    static void RenderSilence(float* start, int32_t numSamples);
    virtual void Render(float* targetData, int32_t numSamples);
    virtual void Play();
    void Stop();
    void Loop(bool isLooping);

    float volume;

protected:
    WavAudioSource* wavAudioSource_;
    int32_t readFrameIndex_;
    std::atomic<bool> isPlaying_;
    std::atomic<bool> isLooping_;
};
}
