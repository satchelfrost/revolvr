/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#pragma once

#include <ecs/component/types/audio.h>
#include <audio/circular_buffer.h>

namespace rvr {
class SpatialAudio : public Audio {
public:
    SpatialAudio(const SpatialAudio& other) = delete;
    SpatialAudio(const SpatialAudio& other, type::EntityId newEntityId);
    virtual Component* Clone(type::EntityId newEntityId) override;


    SpatialAudio(type::EntityId pId, const WavAudioSource& wavAudioSource);
    virtual void Render(float* targetData, int32_t numSamples) override;
    virtual void Play() override;

private:
    void Spatialize();

    CircularBuffer circularBuffer_{8192};
    int32_t totalSourceFrames_;
    const float* data_;
};
}
