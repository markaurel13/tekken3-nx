#include "switch_audio.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <malloc.h>

// Forward declaration de la SPU de psxrecomp
extern "C" {
    void spu_render(int16_t* out_stereo, int frames);
}

SwitchAudio& SwitchAudio::Instance() {
    static SwitchAudio s_instance;
    return s_instance;
}

bool SwitchAudio::Init(int sampleRate) {
    if (m_initialized) return true;

    m_sampleRate = sampleRate;
    printf("[SwitchAudio] Inicializando subsistema audren a %d Hz (Estéreo)...\n", m_sampleRate);

    // 1. Configuración del Audio Renderer nativo
    static const AudioRendererConfig arConfig = {
        .output_rate     = AudioRendererOutputRate_48kHz,
        .num_voices      = 4,
        .num_effects     = 0,
        .num_sinks       = 1,
        .num_mix_objs    = 1,
        .num_mix_buffers = 2,
    };

    Result res = audrenInitialize(&arConfig);
    if (R_FAILED(res)) {
        printf("[SwitchAudio] Error: audrenInitialize falló con código 0x%08X\n", res);
        return false;
    }

    res = audrvCreate(&m_drv, &arConfig, 2);
    if (R_FAILED(res)) {
        printf("[SwitchAudio] Error: audrvCreate falló con código 0x%08X\n", res);
        audrenExit();
        return false;
    }

    // 2. Asignar MemPool alineado a 0x1000 para el hardware de audio
    size_t totalAudioSize = BufferSizeBytes * NumBuffers;
    m_memPoolSize = (totalAudioSize + 0xFFF) & ~0xFFF;
    m_memPoolPtr = memalign(0x1000, m_memPoolSize);
    if (!m_memPoolPtr) {
        printf("[SwitchAudio] Error: no se pudo asignar memoria para mempool\n");
        audrvClose(&m_drv);
        audrenExit();
        return false;
    }
    memset(m_memPoolPtr, 0, m_memPoolSize);
    armDCacheFlush(m_memPoolPtr, m_memPoolSize);

    m_memPoolId = audrvMemPoolAdd(&m_drv, m_memPoolPtr, m_memPoolSize);
    audrvMemPoolAttach(&m_drv, m_memPoolId);

    // 3. Configurar Sink de salida para los 2 canales estéreo
    static const uint8_t sinkChannels[] = { 0, 1 };
    audrvDeviceSinkAdd(&m_drv, AUDREN_DEFAULT_DEVICE_NAME, 2, sinkChannels);

    res = audrvUpdate(&m_drv);
    if (R_FAILED(res)) {
        printf("[SwitchAudio] Error: audrvUpdate inicial falló\n");
    }

    res = audrenStartAudioRenderer();
    if (R_FAILED(res)) {
        printf("[SwitchAudio] Error: audrenStartAudioRenderer falló con código 0x%08X\n", res);
    }

    // 4. Inicializar Voz 0: 2 canales, 16-bit PCM, frecuencia nativa de PS1 (44100 Hz)
    audrvVoiceInit(&m_drv, 0, Channels, PcmFormat_Int16, m_sampleRate);
    audrvVoiceSetDestinationMix(&m_drv, 0, AUDREN_FINAL_MIX_ID);

    // Matriz de paneo estéreo: canal L a Out 0, canal R a Out 1
    audrvVoiceSetMixFactor(&m_drv, 0, 1.0f, 0, 0); // L -> L
    audrvVoiceSetMixFactor(&m_drv, 0, 0.0f, 0, 1); // L -> R
    audrvVoiceSetMixFactor(&m_drv, 0, 0.0f, 1, 0); // R -> L
    audrvVoiceSetMixFactor(&m_drv, 0, 1.0f, 1, 1); // R -> R
    audrvVoiceStart(&m_drv, 0);

    // 5. Configurar WaveBuffers siguiendo el contrato estricto de alineamiento de audren:
    // data_raw SIEMPRE apunta al inicio del mempool (alineado a 0x1000) y cada bloque se
    // indexa mediante start_sample_offset y end_sample_offset.
    int16_t* baseSamples = (int16_t*)m_memPoolPtr;
    for (int i = 0; i < NumBuffers; i++) {
        m_audioBuffers[i] = baseSamples + (i * BufferSamples);
        m_waveBufs[i].data_raw = m_memPoolPtr;
        m_waveBufs[i].size = NumBuffers * BufferSizeBytes;
        m_waveBufs[i].start_sample_offset = i * FramesPerStep;
        m_waveBufs[i].end_sample_offset = (i + 1) * FramesPerStep;
        m_waveBufs[i].is_looping = false;
        m_waveBufs[i].state = AudioDriverWaveBufState_Free;
    }

    m_initialized = true;
    m_currentBuffer = 0;
    printf("[SwitchAudio] Sistema de audio nativo listo (búferes: %d, muestras/frame: %d).\n", NumBuffers, FramesPerStep);
    return true;
}

void SwitchAudio::Update() {
    if (!m_initialized) return;
    static uint64_t s_update_count = 0;
    static uint64_t s_total_update_ns = 0;
    static uint64_t s_max_update_ns = 0;
    static uint64_t s_total_queued = 0;
    u64 start_tick = armGetSystemTick();

    // Actualizar estado del driver para liberar buffers completados
    audrvUpdate(&m_drv);

    // Contar cuántos buffers siguen activos en cola de hardware
    int activeBuffers = 0;
    for (int i = 0; i < NumBuffers; i++) {
        if (m_waveBufs[i].state == AudioDriverWaveBufState_Playing ||
            m_waveBufs[i].state == AudioDriverWaveBufState_Queued ||
            m_waveBufs[i].state == AudioDriverWaveBufState_Waiting) {
            activeBuffers++;
        }
    }

    // Colchón dinámico anti-underrun:
    // Mantener 2-3 buffers (33-50 ms) encolados en hardware para absorber fluctuaciones de framerate
    // sin introducir latencia perceptible ni clicks/distorsión por agotamiento de cola (underrun).
    int blocksToFill;
    if (activeBuffers == 0) {
        blocksToFill = 3; // Cola vacía: rearmar colchón completo (50 ms)
    } else if (activeBuffers == 1) {
        blocksToFill = 2; // Cola baja: recuperar nivel óptimo
    } else if (activeBuffers <= 3) {
        blocksToFill = 1; // Régimen estable 60 FPS: 1 buffer por cuadro
    } else {
        blocksToFill = 0; // Cola llena (>= 4): drenar para evitar acumulación de retardo
    }
    bool queuedAny = false;
    alignas(64) int16_t tempPcm[BufferSamples];

    for (int b = 0; b < blocksToFill; b++) {
        AudioDriverWaveBuf* curWb = &m_waveBufs[m_currentBuffer];
        if (curWb->state != AudioDriverWaveBufState_Free &&
            curWb->state != AudioDriverWaveBufState_Done) {
            break;
        }

        // Sintetizar muestras en caché rápida de CPU y copiar al mempool
        spu_render(tempPcm, FramesPerStep);
        memcpy(m_audioBuffers[m_currentBuffer], tempPcm, BufferSizeBytes);

        curWb->data_raw = m_memPoolPtr;
        curWb->size = NumBuffers * BufferSizeBytes;
        curWb->start_sample_offset = m_currentBuffer * FramesPerStep;
        curWb->end_sample_offset = (m_currentBuffer + 1) * FramesPerStep;

        audrvVoiceAddWaveBuf(&m_drv, 0, curWb);
        m_currentBuffer = (m_currentBuffer + 1) % NumBuffers;
        queuedAny = true;
    }

    if (queuedAny) {
        // Volcar el mempool completo alineado a 0x1000 para garantizar coherencia total con el DSP
        armDCacheFlush(m_memPoolPtr, m_memPoolSize);
        if (!audrvVoiceIsPlaying(&m_drv, 0)) {
            audrvVoiceStart(&m_drv, 0);
        }
        audrvUpdate(&m_drv);
    }

    u64 end_tick = armGetSystemTick();
    uint64_t update_ns = armTicksToNs(end_tick - start_tick);
    s_update_count++;
    s_total_update_ns += update_ns;
    s_total_queued += queuedAny ? 1u : 0u;
    if (update_ns > s_max_update_ns) s_max_update_ns = update_ns;
    if (s_update_count <= 5 || (s_update_count % 120) == 0) {
        double avg_ns = s_update_count ? (double)s_total_update_ns / (double)s_update_count : 0.0;
        printf("[AudioPerf] samples=%llu avg_ns=%.0f max_ns=%llu active=%d queued_total=%llu cur=%d\n",
               (unsigned long long)s_update_count,
               avg_ns,
               (unsigned long long)s_max_update_ns,
               activeBuffers,
               (unsigned long long)s_total_queued,
               m_currentBuffer);
    }
}

void SwitchAudio::QueueFrames(const int16_t* pcmStereo, size_t frames) {
    if (!m_initialized || !pcmStereo || frames == 0) return;

    u64 start_tick = armGetSystemTick();
    u64 spu_start_tick = 0;
    u64 spu_end_tick = 0;
    static uint64_t s_queue_count = 0;
    static uint64_t s_total_queue_ns = 0;
    static uint64_t s_max_queue_ns = 0;
    static uint64_t s_total_spu_ns = 0;
    static uint64_t s_max_spu_ns = 0;
    audrvUpdate(&m_drv);

    AudioDriverWaveBuf* curWb = &m_waveBufs[m_currentBuffer];
    if (curWb->state == AudioDriverWaveBufState_Free || curWb->state == AudioDriverWaveBufState_Done) {
        size_t copyFrames = (frames < (size_t)FramesPerStep) ? frames : (size_t)FramesPerStep;
        size_t copyBytes = copyFrames * Channels * sizeof(int16_t);

        spu_start_tick = armGetSystemTick();
        spu_render(m_audioBuffers[m_currentBuffer], (int)copyFrames);
        spu_end_tick = armGetSystemTick();
        memcpy(m_audioBuffers[m_currentBuffer], pcmStereo, copyBytes);
        armDCacheFlush(m_memPoolPtr, m_memPoolSize);

        curWb->data_raw = m_memPoolPtr;
        curWb->size = NumBuffers * BufferSizeBytes;
        curWb->start_sample_offset = m_currentBuffer * FramesPerStep;
        curWb->end_sample_offset = m_currentBuffer * FramesPerStep + (uint32_t)copyFrames;
        audrvVoiceAddWaveBuf(&m_drv, 0, curWb);

        m_currentBuffer = (m_currentBuffer + 1) % NumBuffers;
        if (!audrvVoiceIsPlaying(&m_drv, 0)) {
            audrvVoiceStart(&m_drv, 0);
        }
        audrvUpdate(&m_drv);

        u64 end_tick = armGetSystemTick();
        uint64_t queue_ns = armTicksToNs(end_tick - start_tick);
        uint64_t spu_ns = armTicksToNs(spu_end_tick - spu_start_tick);
        s_queue_count++;
        s_total_queue_ns += queue_ns;
        s_total_spu_ns += spu_ns;
        if (queue_ns > s_max_queue_ns) s_max_queue_ns = queue_ns;
        if (spu_ns > s_max_spu_ns) s_max_spu_ns = spu_ns;
        if (s_queue_count <= 3 || (s_queue_count % 120) == 0) {
            double avg_queue_ns = s_queue_count ? (double)s_total_queue_ns / (double)s_queue_count : 0.0;
            double avg_spu_ns = s_queue_count ? (double)s_total_spu_ns / (double)s_queue_count : 0.0;
            printf("[AudioPerf] queue samples=%llu avg_ns=%.0f max_ns=%llu avg_spu_ns=%.0f max_spu_ns=%llu frames=%zu copy=%zu buf=%d\n",
                   (unsigned long long)s_queue_count,
                   avg_queue_ns,
                   (unsigned long long)s_max_queue_ns,
                   avg_spu_ns,
                   (unsigned long long)s_max_spu_ns,
                   frames,
                   copyFrames,
                   m_currentBuffer);
        }
    }
}

void SwitchAudio::Shutdown() {
    if (!m_initialized) return;

    audrvVoiceStop(&m_drv, 0);
    audrvClose(&m_drv);
    audrenExit();

    if (m_memPoolPtr) {
        free(m_memPoolPtr);
        m_memPoolPtr = nullptr;
    }

    m_initialized = false;
    printf("[SwitchAudio] Subsistema de audio finalizado.\n");
}

// Interfaz C para psxrecomp
extern "C" {

bool switch_audio_init(int sample_rate) {
    return SwitchAudio::Instance().Init(sample_rate);
}

void switch_audio_update(void) {
    SwitchAudio::Instance().Update();
}

void switch_audio_queue(const int16_t* pcm_stereo, size_t frames) {
    SwitchAudio::Instance().QueueFrames(pcm_stereo, frames);
}

void switch_audio_shutdown(void) {
    SwitchAudio::Instance().Shutdown();
}

}
