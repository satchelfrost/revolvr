#pragma once

#include <aaudio/AAudio.h>
#include <audio/mixer.h>
#include <audio/audio_spatializer.h>
#include <ecs/component/types/audio/spatial_audio.h>

namespace rvr {
class AudioEngine {
public:
    ~AudioEngine();
    bool start();
    void stop();
    void restart();
    void ProcessSpatialAudio(SpatialAudio* spatialAudio, const float* inBuffer, float* outBuffer);
    void SetListenerVectors(const math::Transform& transform);

private:
    AAudioStream *stream_;
    Mixer mixer_;
    AudioSpatializer audioSpatializer_;
};
}