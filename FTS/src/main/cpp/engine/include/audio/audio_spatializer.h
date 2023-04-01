#pragma once

#include <OVR_Audio.h>
#include <ecs/component/types/audio/spatial_audio.h>
#include <math/transform.h>

namespace rvr {
class AudioSpatializer {
public:
    AudioSpatializer();
    ~AudioSpatializer();
    void SetBoxRoomParams(float width, float height, float depth, float reflectance);
    void ProcessSpatialAudio(SpatialAudio* spatialAudio, const float* inBuffer, float* outBuffer);
    void SetListenerVectors(const math::Transform& transform);

private:
    ovrAudioContext audioContext_;
};
}