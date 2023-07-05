/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#pragma once

#include <ecs/component/component.h>
#include <audio/wav_audio_source.h>
#include "audio/circular_buffer.h"

namespace rvr {
class Audio : public Component {
public:
    Audio(const Audio& other) = delete;
    Audio(const Audio& other, type::EntityId pId);
    virtual Component* Clone(type::EntityId newEntityId) override;

    Audio(type::EntityId pId, WavAudioSource wavAudioSource);
    static void RenderSilence(float* start, int32_t numSamples);
    virtual void Render(float* targetData, int32_t numSamples);
    virtual void Play();
    bool IsPlaying();
    void Stop();
    void Loop(bool isLooping);

    float volume;

protected:
    WavAudioSource wavAudioSource_;
    int32_t readFrameIndex_;
    std::atomic<bool> isPlaying_;
    std::atomic<bool> isLooping_;
};
}
