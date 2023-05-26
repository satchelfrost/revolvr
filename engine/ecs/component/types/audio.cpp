#include <ecs/component/types/audio.h>

#include <utility>

namespace rvr {
Audio::Audio(type::EntityId pId, WavAudioSource wavAudioSource) :
Component(ComponentType::Audio, pId), volume(1.0), wavAudioSource_(std::move(wavAudioSource)),
isLooping_(false), isPlaying_(false), readFrameIndex_(0) {}

void Audio::Render(float *targetData, int32_t numSamples) {
    if (isPlaying_) {
        int64_t numSamplesToRender = numSamples;
        int64_t totalSourceFrames  = wavAudioSource_.GetBufferSize();
        const float* data          = wavAudioSource_.GetData();

        if (!isLooping_ && readFrameIndex_ + numSamples >= totalSourceFrames) {
            numSamplesToRender = totalSourceFrames - readFrameIndex_;
            isPlaying_ = false;
        }

        for (int i = 0; i < numSamplesToRender; i++) {
            targetData[i] = data[readFrameIndex_];

            // handle wrap around
            if (++readFrameIndex_ >= totalSourceFrames)
                readFrameIndex_ = 0;
        }

        if (numSamplesToRender < numSamples) {
            // fill remaining buffer with silence
            RenderSilence(&targetData[numSamplesToRender], numSamplesToRender);
        }
    }
    else {
        RenderSilence(targetData, numSamples);
    }
}

void Audio::Play() {
    isPlaying_ = true;
    readFrameIndex_ = 0;
}

void Audio::Stop() {
    isPlaying_ = false;
}

void Audio::Loop(bool isLooping) {
    isLooping_ = isLooping;
}

void Audio::RenderSilence(float *start, int32_t numSamples) {
    for (int i = 0; i < numSamples; i++)
        start[i] = 0;
}

Component *Audio::Clone(type::EntityId newEntityId) {
    return new Audio(*this, newEntityId);
}

Audio::Audio(const Audio &other, type::EntityId pId) :
Component(ComponentType::Audio, pId), volume(other.volume), isLooping_(other.isLooping_.load()),
isPlaying_(other.isPlaying_.load()), readFrameIndex_(other.readFrameIndex_),
wavAudioSource_(other.wavAudioSource_) {}
}
