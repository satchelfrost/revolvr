#include <audio/mixer.h>
#include <global_context.h>
#include <ecs/component/types/audio.h>
#include <ecs/component/types/spatial.h>

namespace rvr {
void Mixer::Render(float* audioData, int32_t numFrames) {
    auto numSamples = numFrames * 2;
    memset(audioData, 0, sizeof(float) * numSamples);
    auto components = GlobalContext::Inst()->GetECS()->GetComponents(ComponentType::Audio);
    for (auto [eid, component] : components) {
        auto track = dynamic_cast<Audio*>(component);
        track->Render(mixingBuffer_, numSamples);

        for (int i = 0; i < numSamples; i++)
            audioData[i] += mixingBuffer_[i] * track->volume;
    }
}
}