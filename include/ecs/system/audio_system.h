/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#pragma once

#include <aaudio/AAudio.h>

namespace rvr::system::audio {
aaudio_data_callback_result_t dataCallback(AAudioStream *stream, void *userData, void *audioData,
                                           int32_t numFrames);

void errorCallback(AAudioStream *stream, void *userData, aaudio_result_t error);

class Mixer {
public:
    void Render(float* audioData, int32_t numFrames);
private:
    float mixingBuffer_[1920];
};
}