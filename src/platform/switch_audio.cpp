#include "switch_audio.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <malloc.h>

// Forward declaration de la SPU de psxrecomp
extern "C" {
    void spu_render(int16_t* out_stereo, int frames);
}

// ---------------------------------------------------------------------------
// DIAGNOSTICO TEMPORAL: por que suenan la musica (bus CD) y no los SFX (voces
// SPU). Solo lectura: consulta el estado de la SPU y los taps de audio, no
// altera ni un registro. Poner TEK_AUDIO_DIAG a 0 (o borrar el bloque) cuando
// se localice la causa.
// ---------------------------------------------------------------------------
#include "spu.h"
#include "audio_trace.h"

#define TEK_AUDIO_DIAG 1

#if TEK_AUDIO_DIAG
/* Espejo exacto de SpuDiagSnapshot (spu.c, solo __SWITCH__). */
extern "C" {
typedef struct SpuDiagSnapshotC {
    uint64_t w_total, w_voice, w_main, w_keyon, w_keyoff, w_cd, w_ctl, w_other;
    uint64_t w_kon0;
    uint32_t kon_seq;
    uint32_t kon_pc[8];
    uint32_t kon_val[8];
    uint32_t top_pc[4];
    uint32_t top_n[4];
} SpuDiagSnapshotC;
void spu_diag_get(SpuDiagSnapshotC* out);
}
static void tek_audio_diag(void)
{
    static unsigned s_call = 0;
    static uint32_t s_prev_keyon = 0;
    static uint64_t s_prev_render = 0;
    static uint64_t s_prev_nz = 0;
    static uint64_t s_prev_vox_aud = 0;
    static uint64_t s_prev_spu_aud = 0;

    if (++s_call % 60u != 0u) return;

    SpuDebugInfo   d;  spu_debug_info(&d);
    SpuGlobalState g;  spu_get_global_state(&g);
    AudioTraceStats ts; audio_trace_get_stats(&ts);

    printf("[SPUDiag] t=%us keyon=+%u/%u active=0x%06X endx=0x%06X ctrl=0x%04X "
           "(en=%u cd=%u rev=%u) main_raw=(0x%04X,0x%04X) main=(%d,%d) cdvol=(%d,%d) "
           "sweepL=0x%06X sweepR=0x%06X sweepMain=0x%X\n",
           s_call / 60u,
           (unsigned)(d.key_on_count - s_prev_keyon), (unsigned)d.key_on_count,
           (unsigned)g.active_mask, (unsigned)g.endx, (unsigned)d.ctrl,
           (unsigned)((d.ctrl >> 15) & 1u), (unsigned)(d.ctrl & 1u),
           (unsigned)((d.ctrl >> 7) & 1u),
           (unsigned)g.main_vol_l, (unsigned)g.main_vol_r,
           (int)d.main_l, (int)d.main_r, (int)d.cd_l, (int)d.cd_r,
           (unsigned)g.sweep_l_mask, (unsigned)g.sweep_r_mask,
           (unsigned)g.sweep_main);

    printf("[SPUMix] render=+%llu/%llu nonzero=+%llu peak_blk=%d peak_hist=%d "
           "cd_push=%llu cd_under=%llu cd_ovf=%llu cd_frames=%u\n",
           (unsigned long long)(d.render_frames - s_prev_render),
           (unsigned long long)d.render_frames,
           (unsigned long long)(d.nonzero_frames - s_prev_nz),
           (int)d.last_peak, (int)d.peak,
           (unsigned long long)d.cd_push_frames,
           (unsigned long long)d.cd_underflow_frames,
           (unsigned long long)d.cd_overflow_frames,
           (unsigned)d.cd_frames);

    /* Taps: audibles = frames con |sample| > 256 (~-42 dBFS). Si el tap de
     * VOCES se queda clavado mientras el de SPU_OUT avanza, las voces no
     * generan senal y lo que se oye es solo el bus CD. */
    printf("[SPUTap] spu_out=+%llu aud=%llu pk=%d | voices=+%llu aud=+%llu/%llu pk=%d | "
           "cd_in=+%llu aud=%llu pk=%d\n",
           (unsigned long long)(ts.tap_frames[AUDIO_TAP_SPU_OUT] - s_prev_render),
           (unsigned long long)(ts.tap_audible[AUDIO_TAP_SPU_OUT] - s_prev_spu_aud),
           ts.tap_peak[AUDIO_TAP_SPU_OUT],
           (unsigned long long)ts.tap_frames[AUDIO_TAP_VOICES],
           (unsigned long long)(ts.tap_audible[AUDIO_TAP_VOICES] - s_prev_vox_aud),
           (unsigned long long)ts.tap_audible[AUDIO_TAP_VOICES],
           ts.tap_peak[AUDIO_TAP_VOICES],
           (unsigned long long)ts.tap_frames[AUDIO_TAP_CD_IN],
           (unsigned long long)ts.tap_audible[AUDIO_TAP_CD_IN],
           ts.tap_peak[AUDIO_TAP_CD_IN]);

    /* Banco de samples en SPU RAM: si esta todo a cero es que el juego nunca
     * subio su banco (DMA4) y las voces solo pueden reproducir silencio. */
    {
        const uint8_t* ram = spu_get_ram();
        uint32_t nz = 0;
        if (ram) for (uint32_t i = 0; i < 512u * 1024u; i++) if (ram[i]) nz++;
        printf("[SPURam] nonzero=%u/%u\n", (unsigned)nz, 512u * 1024u);
    }

    /* Voces vivas: env>0 y volumen distinto de cero = deberia oirse. Si no
     * hay ninguna activa en los menus, el juego no esta pidiendo SFX. */
    {
        int shown = 0;
        for (int v = 0; v < 24; v++) {
            SpuVoiceState vs;
            spu_get_voice_state(v, &vs);
            if (!vs.active && vs.env_level == 0) continue;
            const uint8_t* ram = spu_get_ram();
            const uint8_t* blk = ram ? ram + (vs.cur_addr & (512u * 1024u - 1u)) : nullptr;
            printf("[SPUVox] v%d act=%d adsr=%d env=%04X vol=(%d,%d) pitch=%04X "
                   "cur=%05X loop=%05X adpcm=%02X%02X%02X%02X%02X%02X%02X%02X\n",
                   v, vs.active, (int)vs.adsr_phase, (unsigned)vs.env_level,
                   (int)vs.vol_cur_l, (int)vs.vol_cur_r, (unsigned)vs.pitch,
                   (unsigned)vs.cur_addr, (unsigned)vs.repeat_addr,
                   blk ? blk[0] : 0u, blk ? blk[1] : 0u, blk ? blk[2] : 0u,
                   blk ? blk[3] : 0u, blk ? blk[4] : 0u, blk ? blk[5] : 0u,
                   blk ? blk[6] : 0u, blk ? blk[7] : 0u);
            if (++shown >= 6) break;
        }
        if (!shown) printf("[SPUVox] (ninguna voz activa)\n");
    }

    /* Quien escribe en la SPU: si el driver de sonido del juego esta vivo,
     * w_voice sube cada frame (actualiza volumen/pitch/ADSR de las voces).
     * Si w_total se queda en 0 mientras hay musica, el driver no se ejecuta
     * y por eso nunca llega un KEYON de SFX. */
    {
        SpuDiagSnapshotC sd; spu_diag_get(&sd);
        static uint64_t p_tot = 0, p_voice = 0, p_main = 0, p_kon = 0,
                        p_koff = 0, p_ctl = 0, p_cd = 0, p_oth = 0, p_kon0 = 0;
        printf("[SPUWr] tot=+%llu voice=+%llu main=+%llu keyon(nz)=+%llu kon0=+%llu "
               "koff=+%llu ctl=+%llu cd=+%llu other=+%llu kon_total=%u\n",
               (unsigned long long)(sd.w_total - p_tot),
               (unsigned long long)(sd.w_voice - p_voice),
               (unsigned long long)(sd.w_main - p_main),
               (unsigned long long)(sd.w_keyon - p_kon),
               (unsigned long long)(sd.w_kon0 - p_kon0),
               (unsigned long long)(sd.w_keyoff - p_koff),
               (unsigned long long)(sd.w_ctl - p_ctl),
               (unsigned long long)(sd.w_cd - p_cd),
               (unsigned long long)(sd.w_other - p_oth),
               (unsigned)sd.kon_seq);
        printf("[SPUWr] topPC %08X:%u %08X:%u %08X:%u %08X:%u\n",
               (unsigned)sd.top_pc[0], (unsigned)sd.top_n[0],
               (unsigned)sd.top_pc[1], (unsigned)sd.top_n[1],
               (unsigned)sd.top_pc[2], (unsigned)sd.top_n[2],
               (unsigned)sd.top_pc[3], (unsigned)sd.top_n[3]);
        p_tot = sd.w_total; p_voice = sd.w_voice; p_main = sd.w_main;
        p_kon = sd.w_keyon; p_koff = sd.w_keyoff; p_ctl = sd.w_ctl;
        p_cd = sd.w_cd; p_oth = sd.w_other; p_kon0 = sd.w_kon0;
    }

    s_prev_keyon   = d.key_on_count;
    s_prev_render  = d.render_frames;
    s_prev_nz      = d.nonzero_frames;
    s_prev_vox_aud = ts.tap_audible[AUDIO_TAP_VOICES];
    s_prev_spu_aud = ts.tap_audible[AUDIO_TAP_SPU_OUT];
}
#endif

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

    // Matriz de paneo estéreo con margen de seguridad (-1.9 dB headroom) para evitar
    // clipping digital a 0 dBFS e inter-sample overs en la salida HDMI (TV / Dock):
    // canal L a Out 0, canal R a Out 1
    audrvVoiceSetMixFactor(&m_drv, 0, 0.80f, 0, 0); // L -> L
    audrvVoiceSetMixFactor(&m_drv, 0, 0.00f, 0, 1); // L -> R
    audrvVoiceSetMixFactor(&m_drv, 0, 0.00f, 1, 0); // R -> L
    audrvVoiceSetMixFactor(&m_drv, 0, 0.80f, 1, 1); // R -> R
    audrvVoiceStart(&m_drv, 0);
    printf("[SwitchAudio] Headroom de salida configurado a 0.80f (-1.9 dB para TV/HDMI studio-clean)\n");

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
    m_ringWriteIdx.store(0, std::memory_order_relaxed);
    m_ringReadIdx.store(0, std::memory_order_relaxed);

    // Iniciar hilo de audio dedicado en Core 2 (prioridad 0x2D)
    m_threadRunning = true;
    Result rcThr = threadCreate(&m_audioThread, AudioThreadEntry, this, nullptr, 0x8000, 0x2D, 2);
    if (R_SUCCEEDED(rcThr)) {
        rcThr = threadStart(&m_audioThread);
        if (R_SUCCEEDED(rcThr)) {
            m_threadCreated = true;
            printf("[SwitchAudio] Hilo dedicado de audio activo en Core 2 (prioridad 0x2D, anillo %d slots)\n", RingSlots);
        } else {
            threadClose(&m_audioThread);
            printf("[SwitchAudio] Advertencia: threadStart fallo con codigo 0x%08X\n", rcThr);
        }
    } else {
        printf("[SwitchAudio] Advertencia: threadCreate fallo con codigo 0x%08X\n", rcThr);
    }

    printf("[SwitchAudio] Sistema de audio nativo listo (búferes: %d, muestras/frame: %d).\n", NumBuffers, FramesPerStep);
    return true;
}

extern "C" {
uint64_t g_prof_audio_spu_ns = 0;
uint64_t g_prof_audio_audren_ns = 0;
uint64_t g_prof_audio_underruns = 0;
}

void SwitchAudio::AudioThreadEntry(void* arg) {
    SwitchAudio* self = static_cast<SwitchAudio*>(arg);
    alignas(64) int16_t last_block[BufferSamples] = {0};
    bool has_last_block = false;
    bool primed = false;
    float current_pitch = 1.0f;

    uint32_t health_tick_cnt = 0;
    uint32_t min_occ_win = RingSlots;
    uint32_t max_occ_win = 0;
    int16_t peak_l_win = 0;
    int16_t peak_r_win = 0;
    uint64_t clipped_samples_win = 0;

    // --- Diagnostico: temporizacion de ticks de audren y tasa real de consumo ---
    u64 prev_tick = 0;
    double dt_min_ms = 1e9, dt_max_ms = 0.0, dt_sum_ms = 0.0;
    uint32_t dt_over8 = 0;
    u32 prev_played = audrvVoiceGetPlayedSampleCount(&self->m_drv, 0);

    while (self->m_threadRunning) {
        // audrenWaitFrame espera el tick de 5ms del DSP de audio de Switch
        audrenWaitFrame();
        if (!self->m_threadRunning) break;

        u64 t0 = armGetSystemTick();
        if (prev_tick) {
            double dt = (double)armTicksToNs(t0 - prev_tick) / 1e6;
            if (dt < dt_min_ms) dt_min_ms = dt;
            if (dt > dt_max_ms) dt_max_ms = dt;
            dt_sum_ms += dt;
            if (dt > 8.0) dt_over8++;
        }
        prev_tick = t0;

        audrvUpdate(&self->m_drv);

        int activeBuffers = 0;
        for (int i = 0; i < NumBuffers; i++) {
            if (self->m_waveBufs[i].state == AudioDriverWaveBufState_Playing ||
                self->m_waveBufs[i].state == AudioDriverWaveBufState_Queued ||
                self->m_waveBufs[i].state == AudioDriverWaveBufState_Waiting) {
                activeBuffers++;
            }
        }

        uint32_t r = self->m_ringReadIdx.load(std::memory_order_relaxed);
        uint32_t w = self->m_ringWriteIdx.load(std::memory_order_acquire);
        uint32_t occupancy = (w >= r) ? (w - r) : 0;

        if (occupancy < min_occ_win) min_occ_win = occupancy;
        if (occupancy > max_occ_win) max_occ_win = occupancy;

        if (++health_tick_cnt >= 200) {
            u32 voice_drops = audrvVoiceGetVoiceDropsCount(&self->m_drv, 0);
            printf("[AudioHealth] peak=(L:%d, R:%d) clip=%llu occ=[min=%u, max=%u] pitch=%.4f voice_drops=%u underruns=%llu\n",
                   peak_l_win, peak_r_win, (unsigned long long)clipped_samples_win,
                   min_occ_win, max_occ_win, current_pitch, voice_drops,
                   (unsigned long long)g_prof_audio_underruns);
            u32 played = audrvVoiceGetPlayedSampleCount(&self->m_drv, 0);
            AppletOperationMode opm = appletGetOperationMode();
            printf("[AudioTick] mode=%s dt_ms=[min=%.2f avg=%.2f max=%.2f >8ms=%u] played_delta=%u (esperado~44100) wavebuf_seq=%u\n",
                   opm == AppletOperationMode_Console ? "DOCK" : "HANDHELD",
                   dt_min_ms, dt_sum_ms / 199.0, dt_max_ms, dt_over8,
                   (unsigned)(played - prev_played), audrvVoiceGetWaveBufSeq(&self->m_drv, 0));
            prev_played = played;
            dt_min_ms = 1e9; dt_max_ms = 0.0; dt_sum_ms = 0.0; dt_over8 = 0;
            health_tick_cnt = 0;
            min_occ_win = RingSlots;
            max_occ_win = 0;
            peak_l_win = 0;
            peak_r_win = 0;
            clipped_samples_win = 0;
        }

        // 1. Priming inicial: acumular al menos 16 bloques (~266 ms) antes de comenzar a alimentar audren.
        // Al entregar 3 a audren, quedan 13 buffers completos de colchón elástico (~216 ms).
        if (!primed) {
            if (occupancy >= 16) {
                primed = true;
            } else {
                continue;
            }
        }

        // 2. Control de latencia máxima (sin alterar el tono/pitch):
        // En Nintendo Switch bloqueada a 60 FPS, el juego genera exactamente 44100 muestras/seg
        // y audren consume exactamente 44100 muestras/seg (1:1 lockstep físico por hardware).
        // El tono se mantiene SIEMPRE FIJO a 1.0000x (sin desafinaciones ni efecto cinta gastada).
        // Si tras saltar cinemáticas o pausas largas se acumulan más de 24 slots (>400 ms),
        // descartamos suavemente el exceso para devolver la latencia al colchón óptimo (~200 ms).
        if (occupancy > 24) {
            uint32_t drop_count = occupancy - 16;
            r += drop_count;
            self->m_ringReadIdx.store(r, std::memory_order_release);
        }

        bool queuedAny = false;
        while (activeBuffers < 3) {
            AudioDriverWaveBuf* curWb = &self->m_waveBufs[self->m_currentBuffer];
            if (curWb->state != AudioDriverWaveBufState_Free &&
                curWb->state != AudioDriverWaveBufState_Done) {
                break;
            }

            r = self->m_ringReadIdx.load(std::memory_order_relaxed);
            w = self->m_ringWriteIdx.load(std::memory_order_acquire);

            if (r < w) {
                uint32_t slot = r % RingSlots;
                const int16_t* s_ptr = self->m_ring[slot].samples;
                for (size_t s = 0; s < BufferSamples; s += 2) {
                    int16_t sl = s_ptr[s];
                    int16_t sr = s_ptr[s + 1];
                    int16_t al = (sl < 0) ? (int16_t)-sl : sl;
                    int16_t ar = (sr < 0) ? (int16_t)-sr : sr;
                    if (al > peak_l_win) peak_l_win = al;
                    if (ar > peak_r_win) peak_r_win = ar;
                    if (al >= 32700 || ar >= 32700) clipped_samples_win++;
                }
                memcpy(self->m_audioBuffers[self->m_currentBuffer], s_ptr, BufferSizeBytes);
                memcpy(last_block, s_ptr, BufferSizeBytes);
                has_last_block = true;
                self->m_ringReadIdx.store(r + 1, std::memory_order_release);
            } else {
                g_prof_audio_underruns++;
                if (has_last_block) {
                    for (size_t s = 0; s < BufferSamples; s++) {
                        last_block[s] = (int16_t)(last_block[s] * 7 / 8);
                    }
                    memcpy(self->m_audioBuffers[self->m_currentBuffer], last_block, BufferSizeBytes);
                } else {
                    memset(self->m_audioBuffers[self->m_currentBuffer], 0, BufferSizeBytes);
                }
            }

            curWb->data_raw = self->m_memPoolPtr;
            curWb->size = NumBuffers * BufferSizeBytes;
            curWb->start_sample_offset = self->m_currentBuffer * FramesPerStep;
            curWb->end_sample_offset = (self->m_currentBuffer + 1) * FramesPerStep;

            audrvVoiceAddWaveBuf(&self->m_drv, 0, curWb);
            self->m_currentBuffer = (self->m_currentBuffer + 1) % NumBuffers;
            activeBuffers++;
            queuedAny = true;
        }

        if (queuedAny) {
            armDCacheFlush(self->m_memPoolPtr, self->m_memPoolSize);
            if (!audrvVoiceIsPlaying(&self->m_drv, 0)) {
                audrvVoiceStart(&self->m_drv, 0);
            }
            audrvUpdate(&self->m_drv);
        }

        u64 t1 = armGetSystemTick();
        g_prof_audio_audren_ns += armTicksToNs(t1 - t0);
    }
}

void SwitchAudio::Update() {
    if (!m_initialized) return;

    u64 t_spu0 = armGetSystemTick();
    alignas(64) int16_t tempPcm[BufferSamples];
    spu_render(tempPcm, FramesPerStep);
    u64 t_spu1 = armGetSystemTick();
    uint64_t spu_ns = armTicksToNs(t_spu1 - t_spu0);
    g_prof_audio_spu_ns += spu_ns;

    if (m_threadCreated) {
        // Encolar en el anillo lock-free sin bloqueos
        uint32_t w = m_ringWriteIdx.load(std::memory_order_relaxed);
        uint32_t r = m_ringReadIdx.load(std::memory_order_acquire);

        if (w - r >= (uint32_t)RingSlots) {
            m_ringReadIdx.store(r + 1, std::memory_order_release);
        }

        uint32_t slot = w % RingSlots;
        memcpy(m_ring[slot].samples, tempPcm, BufferSizeBytes);
        m_ring[slot].frames = FramesPerStep;
        m_ringWriteIdx.store(w + 1, std::memory_order_release);
    } else {
        // Fallback síncrono en caso de que no haya hilo de audio
        audrvUpdate(&m_drv);
        AudioDriverWaveBuf* curWb = &m_waveBufs[m_currentBuffer];
        if (curWb->state == AudioDriverWaveBufState_Free || curWb->state == AudioDriverWaveBufState_Done) {
            memcpy(m_audioBuffers[m_currentBuffer], tempPcm, BufferSizeBytes);
            armDCacheFlush(m_memPoolPtr, m_memPoolSize);
            curWb->data_raw = m_memPoolPtr;
            curWb->size = NumBuffers * BufferSizeBytes;
            curWb->start_sample_offset = m_currentBuffer * FramesPerStep;
            curWb->end_sample_offset = (m_currentBuffer + 1) * FramesPerStep;
            audrvVoiceAddWaveBuf(&m_drv, 0, curWb);
            m_currentBuffer = (m_currentBuffer + 1) % NumBuffers;
            if (!audrvVoiceIsPlaying(&m_drv, 0)) {
                audrvVoiceStart(&m_drv, 0);
            }
            audrvUpdate(&m_drv);
        }
    }

#if TEK_AUDIO_DIAG
    tek_audio_diag();
#endif
}

void SwitchAudio::QueueFrames(const int16_t* pcmStereo, size_t frames) {
    if (!m_initialized || !pcmStereo || frames == 0) return;

    size_t copyFrames = (frames < (size_t)FramesPerStep) ? frames : (size_t)FramesPerStep;
    alignas(64) int16_t temp[BufferSamples];
    memcpy(temp, pcmStereo, copyFrames * Channels * sizeof(int16_t));

    if (m_threadCreated) {
        uint32_t w = m_ringWriteIdx.load(std::memory_order_relaxed);
        uint32_t r = m_ringReadIdx.load(std::memory_order_acquire);
        if (w - r >= (uint32_t)RingSlots) {
            m_ringReadIdx.store(r + 1, std::memory_order_release);
        }
        uint32_t slot = w % RingSlots;
        memcpy(m_ring[slot].samples, temp, BufferSizeBytes);
        m_ring[slot].frames = (uint32_t)copyFrames;
        m_ringWriteIdx.store(w + 1, std::memory_order_release);
    }
}

void SwitchAudio::Shutdown() {
    if (!m_initialized) return;

    if (m_threadCreated) {
        m_threadRunning = false;
        threadWaitForExit(&m_audioThread);
        threadClose(&m_audioThread);
        m_threadCreated = false;
        printf("[SwitchAudio] Hilo dedicado de audio finalizado ordenadamente.\n");
    }

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
