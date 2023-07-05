/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#pragma once
#include <pch.h>

namespace rvr {
typedef int RiffInt32;
typedef short RiffInt16;

static const short ENCODING_PCM = 1;
static const short ENCODING_ADPCM = 2;
static const short ENCODING_IEEE_FLOAT = 3;

struct WavInfo {
    // Reusable tag
    RiffInt32 tag;

    // RIFF chunk
    RiffInt32 fileSize;

    // WAVE chunk
    RiffInt32 wavSize;
    RiffInt16 encodingId;
    RiffInt16 numChannels;
    RiffInt32 sampleRate;
    RiffInt32 bytesPerSec;
    RiffInt16 blockAlign;
    RiffInt16 sampleSize;
    RiffInt16 extraBytes;

    // Data chunk
    RiffInt32 dataSize;

    // All other chunks
    RiffInt32 chunkSize;
};

class WavAudioSource {
public:
    WavAudioSource(const std::string& fileName, bool stereoOutput);
    const float* GetData();
    int32_t GetBufferSize();
    void LogWavInfo();

    const bool stereo;

private:
    void Read(void *chunk, unsigned int chunkSize);
    void ParseWavHeader();
    std::vector<float> buffer_;
    WavInfo wavInfo_;

    char* fileContent_;     // raw wav file loaded as bytes
    unsigned int pos_;  // marks the position in the raw file

    // Setup some constants
    static constexpr float kSampleFullScale = (float) 0x8000;
    static constexpr float kInverseScale = 1.0f / kSampleFullScale;
};
}