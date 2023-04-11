#include <audio/audio_spatializer.h>
#include <common.h>
#include <global_context.h>
#include <ecs/component/types/spatial.h>

namespace rvr {
AudioSpatializer::AudioSpatializer() {
    ovrAudioContextConfiguration config{};
    config.acc_Size = sizeof(config);
    config.acc_SampleRate = 48000;
    config.acc_BufferLength = 192;
    config.acc_MaxNumSources = 1;

    if (ovrAudio_CreateContext(&audioContext_, &config) != ovrSuccess)
        THROW("Could not create ovr audio context");

    // Enable simple room modeling with reverb
    ovrAudio_Enable(audioContext_, ovrAudioEnable_SimpleRoomModeling, 1);
    ovrAudio_Enable(audioContext_, ovrAudioEnable_LateReverberation, 1);
    ovrAudio_Enable(audioContext_, ovrAudioEnable_RandomizeReverb, 1);

    SetBoxRoomParams(10.0f, 10.0f, 10.0f, 1.0f);
//    SetBoxRoomParams(5.0f, 5.0f, 5.0f, 1.0f);
    ovrAudio_SetAudioSourceAttenuationMode(audioContext_, 0,
                                           ovrAudioSourceAttenuationMode_InverseSquare, 1.0f);

    ovrAudio_SetAudioSourceRange(audioContext_, 0, 1.0f, 1000.0f);
}

void AudioSpatializer::SetBoxRoomParams(float width, float height, float depth, float reflectance) {
    ovrAudioBoxRoomParameters brp = {};

    brp.brp_Size = sizeof( brp );
    brp.brp_ReflectLeft  = brp.brp_ReflectRight  =
    brp.brp_ReflectUp    = brp.brp_ReflectDown   =
    brp.brp_ReflectFront = brp.brp_ReflectBehind = reflectance;

    brp.brp_Width  = width;
    brp.brp_Height = height;
    brp.brp_Depth  = depth;

    ovrAudio_SetSimpleBoxRoomParameters(audioContext_, &brp);
}

void AudioSpatializer::ProcessSpatialAudio(SpatialAudio* spatialAudio, const float *inBuffer, float *outBuffer) {
    uint32_t status;
    auto spatial = GlobalContext::Inst()->GetECS()->GetComponent<Spatial>(spatialAudio->id);
    auto pos = spatial->GetWorld().GetPosition();

    ovrAudio_SetAudioSourcePos(audioContext_, 0, pos.x, pos.y, pos.z);
    ovrAudio_SpatializeMonoSourceInterleaved(audioContext_, 0, &status, outBuffer,
                                             inBuffer);
}

AudioSpatializer::~AudioSpatializer() {
    ovrAudio_DestroyContext(audioContext_);
    ovrAudio_Shutdown();
}

void AudioSpatializer::SetListenerVectors(const math::Transform& transform) {
    auto pos     = transform.GetPosition();
    auto up      = transform.GetYAxis();
    auto forward = -transform.GetZAxis();
    ovrAudio_SetListenerVectors(audioContext_,
                                pos.x, pos.y, pos.z,
                                forward.x, forward.y, forward.z,
                                up.x, up.y, up.z);
}
}