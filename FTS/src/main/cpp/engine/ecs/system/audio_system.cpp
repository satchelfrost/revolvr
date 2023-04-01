#include <ecs/system/audio_system.h>
#include <functional>
#include <thread>
#include <audio/audio_engine.h>

namespace rvr::system::audio {
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