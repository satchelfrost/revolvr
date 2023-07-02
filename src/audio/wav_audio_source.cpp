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

#include <audio/wav_audio_source.h>
#include <global_context.h>
#include <common.h>

// Chunk types
#define RIFF 0x46464952
#define FMT  0x20746D66
#define WAVE 0x45564157
#define DATA 0x61746164

namespace rvr {
WavAudioSource::WavAudioSource(const std::string& fileName, bool stereoOutput) : stereo(stereoOutput), pos_(0) {
    // Load the asset
    std::string assetName = fileName + ".wav";
    auto assetMgr = GlobalContext::Inst()->GetAndroidContext()->GetAndroidAssetManager();
    AAsset* asset = AAssetManager_open(assetMgr, assetName.c_str(), AASSET_MODE_UNKNOWN);
    CHECK_MSG(asset, Fmt("Could not find asset named %s", assetName.c_str()));
    off_t size = AAsset_getLength(asset);
    fileContent_ = new char[size];
    AAsset_read(asset, fileContent_, size);
    AAsset_close(asset);

    // Read the header information
    ParseWavHeader();

    // Read the data section of wav file and convert to RiffInt16 array
    auto bufferSize = wavInfo_.dataSize / 2;
    auto data = new RiffInt16[bufferSize];
    RiffInt16 frame = 0;
    for (int i = 0; i < bufferSize; i++) {
        Read(&frame, sizeof(frame));
        data[i] = frame;
        if (pos_ > size)
            THROW("Pos > size of file. Something went wrong.")
    }

    // Convert RiffInt16 buffer to float buffer
    if ((stereoOutput && wavInfo_.numChannels == 2) ||
        (!stereoOutput && wavInfo_.numChannels == 1)) {
        buffer_.resize(bufferSize);
        for (int i = 0; i < bufferSize; i++)
            buffer_[i] = (float) data[i] * kInverseScale;
    }
    else if (stereoOutput && wavInfo_.numChannels == 1){
        buffer_.resize(bufferSize * 2); // force mono match stereo
        for (int i = 0; i < bufferSize; i++) {
            buffer_[i * 2]     = (float) data[i] * kInverseScale;
            buffer_[i * 2 + 1] = (float) data[i] * kInverseScale;
        }
    }
    else {
        THROW("Either trying to convert stereo to mono, or some other odd configuration");
    }

    delete[] data;
    delete[] fileContent_;
    Log::Write(Log::Level::Info, Fmt("Successfully loaded audio source %s", assetName.c_str()));
}

int32_t WavAudioSource::GetBufferSize() {
    return buffer_.size();
}

const float* WavAudioSource::GetData() {
    return buffer_.data();
}

void WavAudioSource::LogWavInfo() {
    Log::Write(Log::Level::Info, Fmt("File size %d", wavInfo_.fileSize));
    Log::Write(Log::Level::Info, Fmt("Wave size %d", wavInfo_.wavSize));
    Log::Write(Log::Level::Info, Fmt("Encoding id %d", wavInfo_.encodingId));
    Log::Write(Log::Level::Info, Fmt("num channels %d", wavInfo_.numChannels));
    Log::Write(Log::Level::Info, Fmt("Sample rate %d", wavInfo_.sampleRate));
    Log::Write(Log::Level::Info, Fmt("bps %d", wavInfo_.bytesPerSec));
    Log::Write(Log::Level::Info, Fmt("Block align %d", wavInfo_.blockAlign));
    Log::Write(Log::Level::Info, Fmt("Sample size %d", wavInfo_.sampleSize));
    if (wavInfo_.encodingId != ENCODING_PCM && wavInfo_.encodingId != ENCODING_IEEE_FLOAT)
        Log::Write(Log::Level::Info, "Reading extra bytes");
}

void WavAudioSource::ParseWavHeader() {
    bool dataSectionFound = false;
    while (!dataSectionFound) {
        Read(&wavInfo_.tag, sizeof(wavInfo_.tag));
        switch (wavInfo_.tag) {
            case RIFF:
                Read(&wavInfo_.fileSize, sizeof(wavInfo_.fileSize));
                continue;
            case FMT:
                Read(&wavInfo_.wavSize, sizeof(wavInfo_.wavSize));
                Read(&wavInfo_.encodingId,  sizeof(wavInfo_.encodingId));
                Read(&wavInfo_.numChannels, sizeof(wavInfo_.numChannels));
                Read(&wavInfo_.sampleRate,  sizeof(wavInfo_.sampleRate));
                Read(&wavInfo_.bytesPerSec, sizeof(wavInfo_.bytesPerSec));
                Read(&wavInfo_.blockAlign,  sizeof(wavInfo_.blockAlign));
                Read(&wavInfo_.sampleSize,  sizeof(wavInfo_.sampleSize));
                if (wavInfo_.encodingId != ENCODING_PCM && wavInfo_.encodingId != ENCODING_IEEE_FLOAT)
                    Read(&wavInfo_.extraBytes, sizeof(wavInfo_.extraBytes));
                continue;
            case WAVE:
                continue;
            case DATA:
                Read(&wavInfo_.dataSize, sizeof(wavInfo_.dataSize));
                dataSectionFound = true;
                continue;
            default:
                Read(&wavInfo_.chunkSize, sizeof(wavInfo_.chunkSize));
                pos_ += wavInfo_.chunkSize;
                continue;
        }
    }
}

void WavAudioSource::Read(void *chunk, unsigned int chunkSize) {
    memcpy(chunk, fileContent_ + pos_, chunkSize);
    pos_ += chunkSize;
}
}
