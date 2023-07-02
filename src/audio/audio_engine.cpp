/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/

#include <audio/audio_engine.h>
#include <mutex>
#include <common.h>
#include <ecs/system/audio_system.h>

namespace rvr {
// Double-buffering offers a good tradeoff between latency and protection against glitches.
constexpr int32_t kBufferSizeInBursts = 2;

AudioEngine::AudioEngine() {
    if (!start())
        THROW("Failed to start Audio engine");
}

bool AudioEngine::start() {
    AAudioStreamBuilder *streamBuilder;
    AAudio_createStreamBuilder(&streamBuilder);
    AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_FLOAT);
    AAudioStreamBuilder_setChannelCount(streamBuilder, 2);
    AAudioStreamBuilder_setPerformanceMode(streamBuilder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    AAudioStreamBuilder_setDataCallback(streamBuilder, system::audio::dataCallback, &mixer_);
    AAudioStreamBuilder_setErrorCallback(streamBuilder, system::audio::errorCallback, this);

    // Opens the stream.
    aaudio_result_t result = AAudioStreamBuilder_openStream(streamBuilder, &stream_);
    if (result != AAUDIO_OK) {
        Log::Write(Log::Level::Info, Fmt("Error opening stream %s",
                                                     AAudio_convertResultToText(result)));
        return false;
    }

    // Retrieves the sample rate of the stream for our oscillator.
    int32_t sampleRate = AAudioStream_getSampleRate(stream_);

    // Sets the buffer size.
    auto framesPerBurst = AAudioStream_getFramesPerBurst(stream_) * kBufferSizeInBursts;
    AAudioStream_setBufferSizeInFrames(stream_, framesPerBurst);

    // Starts the stream.
    result = AAudioStream_requestStart(stream_);
    if (result != AAUDIO_OK) {
        Log::Write(Log::Level::Info, Fmt("Error starting stream %s", AAudio_convertResultToText(result)));
        return false;
    }

    AAudioStreamBuilder_delete(streamBuilder);
    return true;
}

void AudioEngine::restart(){
    static std::mutex restartingLock;
    if (restartingLock.try_lock()){
        stop();
        start();
        restartingLock.unlock();
    }
}

void AudioEngine::stop() {
    if (stream_ != nullptr) {
        AAudioStream_requestStop(stream_);
        AAudioStream_close(stream_);
    }
}

AudioEngine::~AudioEngine() {
    stop();
}

void AudioEngine::ProcessSpatialAudio(SpatialAudio* spatialAudio, const float *inBuffer,
                                      float *outBuffer) {
    audioSpatializer_.ProcessSpatialAudio(spatialAudio, inBuffer, outBuffer);
}

void AudioEngine::SetHeadId(type::EntityId id) {
    audioSpatializer_.SetHeadId(id);
}

void AudioEngine::ResetHeadForSpatialAudio() {
    audioSpatializer_.ResetHeadForSpatialAudio();
}
}