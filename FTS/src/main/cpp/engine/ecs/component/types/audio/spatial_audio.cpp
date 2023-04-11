#include <ecs/component/types/audio/spatial_audio.h>
#include <global_context.h>
#include "ecs/component/types/spatial.h"

#define IN_BUFFER_SIZE 192
#define OUT_BUFFER_SIZE IN_BUFFER_SIZE * 2

namespace rvr {
SpatialAudio::SpatialAudio(type::EntityId pId, WavAudioSource *wavAudioSource) :
Audio(pId, wavAudioSource) {
    if (wavAudioSource->stereo)
        THROW("Spatial audio must be mono");

    totalSourceFrames_ = wavAudioSource_->GetBufferSize();
    data_ = wavAudioSource_->GetData();
}

void SpatialAudio::Render(float *targetData, int32_t numSamples) {
    ResetHead();

    if (isPlaying_) {
        // Check if we've reached the end of the audio data source
        if (!isLooping_ && readFrameIndex_ + IN_BUFFER_SIZE >= totalSourceFrames_)
            isPlaying_ = false;

        // Populates the circular buffer with spatialized audio
        Spatialize();

        // Handle wrap around
        if (readFrameIndex_ >= totalSourceFrames_)
            readFrameIndex_ = 0;
    }

    for (int i = 0; i < numSamples; i++)
        if(!circularBuffer_.Dequeue(targetData[i]))
            targetData[i] = 0; // render silence if nothing on the queue
}

void SpatialAudio::ResetHead() {
    auto headId = GlobalContext::Inst()->headEntityId;
    auto head = GlobalContext::Inst()->GetECS()->GetComponent<Spatial>(headId);
    GlobalContext::Inst()->GetAudioEngine()->SetListenerVectors(head->GetWorld());
}

void SpatialAudio::Spatialize() {
    float mono[IN_BUFFER_SIZE];
    float stereo[OUT_BUFFER_SIZE];
    memset(mono, 0, sizeof(float) * IN_BUFFER_SIZE);
    memset(stereo, 0, sizeof(float) * OUT_BUFFER_SIZE);

    // Copy the audio data into the mono buffer
    int32_t copyAmount;
    if (readFrameIndex_ + IN_BUFFER_SIZE >= totalSourceFrames_)
        copyAmount = totalSourceFrames_ - readFrameIndex_;
    else
        copyAmount = IN_BUFFER_SIZE;
    memcpy(mono, data_ + readFrameIndex_, sizeof(float) * copyAmount);
    readFrameIndex_ += copyAmount;

    // Spatialize the audio
    if (circularBuffer_.isSpaceAvailable(OUT_BUFFER_SIZE)) {
        GlobalContext::Inst()->GetAudioEngine()->ProcessSpatialAudio(this, mono, stereo);
        for (float i : stereo)
            circularBuffer_.Enqueue(i);
    }
    else {
        Log::Write(Log::Level::Warning, Fmt("Circular audio buffer is full!"));
    }
}

void SpatialAudio::Play() {
    Audio::Play();
    circularBuffer_.Clear();
}
}