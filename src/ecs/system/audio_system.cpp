/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#include <audio/audio_engine.h>
#include <global_context.h>

namespace rvr::system::audio {
void Mixer::Render(float* audioData, int32_t numFrames) {
    auto numSamples = numFrames * 2;
    memset(audioData, 0, sizeof(float) * numSamples);
    auto components = GlobalContext::Inst()->GetECS()->GetComponents(ComponentType::Audio);
    for (auto [eid, component] : components) {
        auto track = dynamic_cast<Audio*>(component);
        if(track) {
            track->Render(mixingBuffer_, numSamples);

            for (int i = 0; i < numSamples; i++)
                audioData[i] += mixingBuffer_[i] * track->volume;
        }
    }
}

aaudio_data_callback_result_t dataCallback(AAudioStream *stream, void *userData, void *audioData,
                                           int32_t numFrames) {
    ((Mixer*) (userData))->Render(static_cast<float*>(audioData), numFrames);
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

void errorCallback(AAudioStream *stream, void *userData, aaudio_result_t error){
    if (error == AAUDIO_ERROR_DISCONNECTED){
        std::function<void(void)> restartFunction = std::bind(&AudioEngine::restart,
                                                              static_cast<AudioEngine*>(userData));
        new std::thread(restartFunction);
    }
}

}