#pragma once

#include <aaudio/AAudio.h>
#include <ecs/system/audio_system.h>
#include <audio/audio_spatializer.h>
#include <ecs/component/types/audio/spatial_audio.h>

namespace rvr {
class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();
    bool start();
    void stop();
    void restart();
    void ProcessSpatialAudio(SpatialAudio* spatialAudio, const float* inBuffer, float* outBuffer);
    void SetHeadId(type::EntityId id);
    void ResetHeadForSpatialAudio();

private:
    AAudioStream *stream_;
    system::audio::Mixer mixer_;
    AudioSpatializer audioSpatializer_;
};
}