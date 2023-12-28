/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

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
    sys::audio::Mixer mixer_;
    AudioSpatializer audioSpatializer_;
};
}