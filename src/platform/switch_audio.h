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

#include <atomic>

class SwitchAudio {
public:
    static constexpr int DefaultSampleRate = 44100;
    static constexpr int Channels = 2;
    static constexpr int FramesPerStep = 735; // 44100 / 60 fps
    static constexpr int NumBuffers = 5;     // 5 buffers para hardware audren
    static constexpr int RingSlots = 48;     // 48 buffers para colchón elástico (~800 ms)
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

    static void AudioThreadEntry(void* arg);

    int m_sampleRate = DefaultSampleRate;
    bool m_initialized = false;

    AudioDriver m_drv{};
    int m_memPoolId = -1;
    void* m_memPoolPtr = nullptr;
    size_t m_memPoolSize = 0;

    AudioDriverWaveBuf m_waveBufs[NumBuffers]{};
    int16_t* m_audioBuffers[NumBuffers]{};
    int m_currentBuffer = 0;

    // Hilo dedicado de audio en Core 2
    Thread m_audioThread{};
    bool m_threadCreated = false;
    volatile bool m_threadRunning = false;

    // Anillo lock-free SPSC para desacoplar el emulador del DSP de Switch
    struct AudioSlot {
        alignas(64) int16_t samples[BufferSamples];
        uint32_t frames;
    };
    AudioSlot m_ring[RingSlots]{};
    alignas(64) std::atomic<uint32_t> m_ringWriteIdx{0};
    alignas(64) std::atomic<uint32_t> m_ringReadIdx{0};
};

#endif
