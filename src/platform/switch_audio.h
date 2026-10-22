#pragma once

#include <stdint.h>
#include <stddef.h>
#include <switch.h>

#ifdef __cplusplus
extern "C" {
#endif

// Funciones C para interfacing con el runtime de psxrecomp
bool switch_audio_init(int sample_rate);
void switch_audio_update(void);
void switch_audio_queue(const int16_t* pcm_stereo, size_t frames);
void switch_audio_shutdown(void);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

class SwitchAudio {
public:
    static constexpr int DefaultSampleRate = 44100;
    static constexpr int Channels = 2;
    static constexpr int FramesPerStep = 735; // 44100 / 60 fps
    static constexpr int NumBuffers = 5;     // 5 buffers para colchón anti-underrun
    static constexpr size_t BufferSamples = FramesPerStep * Channels;
    static constexpr size_t BufferSizeBytes = BufferSamples * sizeof(int16_t);

    static SwitchAudio& Instance();

    bool Init(int sampleRate = DefaultSampleRate);
    void Update();
    void QueueFrames(const int16_t* pcmStereo, size_t frames);
    void Shutdown();

private:
    SwitchAudio() = default;
    ~SwitchAudio() = default;

    int m_sampleRate = DefaultSampleRate;
    bool m_initialized = false;

    AudioDriver m_drv{};
    int m_memPoolId = -1;
    void* m_memPoolPtr = nullptr;
    size_t m_memPoolSize = 0;

    AudioDriverWaveBuf m_waveBufs[NumBuffers]{};
    int16_t* m_audioBuffers[NumBuffers]{};
    int m_currentBuffer = 0;
};

#endif
