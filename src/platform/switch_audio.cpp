#include "switch_audio.h"
#include "audio_limiter.h"
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

#define TEK_AUDIO_DIAG 0

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

    printf("[SwitchAudio] Sistema de audio nativo listo (Baja Latencia Optima ~50ms, anillo 2-3 frames, sonido limpio).\n");
    return true;
}

extern "C" {
uint64_t g_prof_audio_spu_ns = 0;
uint64_t g_prof_audio_audren_ns = 0;
uint64_t g_prof_audio_underruns = 0;
}

// ---------------------------------------------------------------------------
// TELEMETRIA DE AUDIO (Fase 0). El hilo de audio NO imprime nada: solo actualiza
// estos contadores atomicos/planos; el hilo principal los vuelca una vez por
// ventana en switch_audio_telemetry_report().
// ---------------------------------------------------------------------------
extern "C" {
uint64_t g_audio_limiter_calls = 0;
uint64_t g_audio_limiter_active = 0;
}

namespace {
struct AudioTelem {
    // --- Escritos por el hilo principal (Update / spu_render) ---
    uint64_t produced_frames = 0;   // frames de audio (por canal) generados, acumulado
    uint64_t produced_blocks = 0;
    uint64_t ring_overflow = 0;     // bloques descartados por anillo lleno (productor)
    int32_t  raw_peak = 0;          // salida cruda de __real_spu_render (ya con clamp16 del SPU)
    uint64_t raw_clip = 0;          // muestras |x| >= 32700 en la salida cruda
    uint64_t raw_samples = 0;
    int32_t  pre_peak = 0;          // mezcla final antes del limitador de Update()
    uint64_t pre_clip = 0;
    uint64_t pre_over24576 = 0;
    int32_t  post_peak = 0;         // despues del limitador de Update()
    uint64_t post_clip = 0;
    // --- Escritos por el hilo de audio ---
    std::atomic<uint64_t> consumed_blocks{0};
    std::atomic<uint64_t> trim_drops{0};      // bloques descartados por exceso de ocupacion (consumidor)
    std::atomic<uint64_t> ticks{0};
    std::atomic<uint64_t> dt_sum_us{0};
    std::atomic<uint64_t> dt_over8{0};
    std::atomic<uint32_t> dt_max_us{0};
    std::atomic<uint32_t> occ_min{0xFFFFFFFFu};
    std::atomic<uint32_t> occ_max{0};
    std::atomic<uint32_t> occ_cur{0};
    std::atomic<uint32_t> played_samples{0};  // audrvVoiceGetPlayedSampleCount (u32, con wrap)
    std::atomic<uint32_t> voice_drops{0};
};
AudioTelem g_at;

inline void telem_scan(const int16_t* pcm, size_t n, int32_t& peak, uint64_t& clip, uint64_t* over = nullptr) {
    int32_t pk = peak;
    uint64_t cl = clip, ov = 0;
    for (size_t i = 0; i < n; i++) {
        int32_t a = pcm[i];
        if (a < 0) a = -a;
        if (a > pk) pk = a;
        if (a >= 32700) cl++;
        if (a > AUDIO_LIMITER_THRESHOLD) ov++;
    }
    peak = pk;
    clip = cl;
    if (over) *over += ov;
}
} // namespace

extern "C" void switch_audio_telem_raw(const int16_t* out, int frames) {
    if (!out || frames <= 0) return;
    telem_scan(out, (size_t)frames * 2, g_at.raw_peak, g_at.raw_clip);
    g_at.raw_samples += (uint64_t)frames * 2;
}

extern "C" void switch_audio_telemetry_report(void) {
    static u64 s_last_tick = 0;
    static uint64_t l_prod = 0, l_cons = 0, l_prodb = 0, l_ticks = 0, l_dtsum = 0, l_over8 = 0;
    static uint64_t l_underrun = 0, l_lim_calls = 0, l_lim_active = 0, l_ovf = 0, l_trim = 0;
    static uint32_t l_played = 0;
    u64 now = armGetSystemTick();
    if (!s_last_tick) {
        // Primera llamada: solo fija la linea base.
        s_last_tick = now;
        l_played = g_at.played_samples.load(std::memory_order_relaxed);
        l_underrun = g_prof_audio_underruns;
        l_lim_calls = g_audio_limiter_calls; l_lim_active = g_audio_limiter_active;
        g_at.occ_min.store(0xFFFFFFFFu); g_at.occ_max.store(0); g_at.dt_max_us.store(0);
        g_at.raw_peak = g_at.pre_peak = g_at.post_peak = 0;
        g_at.raw_clip = g_at.raw_samples = g_at.pre_clip = g_at.pre_over24576 = g_at.post_clip = 0;
        return;
    }
    double wall = (double)armTicksToNs(now - s_last_tick) / 1e9;
    if (wall < 0.25) return;   // ventana demasiado corta (frames 1..5 del arranque)
    s_last_tick = now;

    uint64_t prod = g_at.produced_frames, prodb = g_at.produced_blocks;
    uint64_t cons = g_at.consumed_blocks.load(std::memory_order_relaxed);
    uint64_t ticks = g_at.ticks.load(std::memory_order_relaxed);
    uint64_t dtsum = g_at.dt_sum_us.load(std::memory_order_relaxed);
    uint64_t over8 = g_at.dt_over8.load(std::memory_order_relaxed);
    uint32_t played = g_at.played_samples.load(std::memory_order_relaxed);
    uint32_t dtmax = g_at.dt_max_us.exchange(0);
    uint32_t omin = g_at.occ_min.exchange(0xFFFFFFFFu);
    uint32_t omax = g_at.occ_max.exchange(0);
    uint32_t ocur = g_at.occ_cur.load(std::memory_order_relaxed);
    uint64_t trim = g_at.trim_drops.load(std::memory_order_relaxed);
    uint64_t underrun = g_prof_audio_underruns;
    uint64_t lcalls = g_audio_limiter_calls, lact = g_audio_limiter_active;

    uint64_t d_ticks = ticks - l_ticks;
    AppletOperationMode opm = appletGetOperationMode();
    ApmPerformanceMode perf = appletGetPerformanceMode();
    printf("[AudioTelem] wall=%.3fs produced=%.0f/s played=%.0f/s (diff=%+.0f/s, esperado 44100) blocks(prod=%llu cons=%llu) "
           "underruns=%llu ring[min=%u max=%u cur=%u/48] overflow(prod=%llu trim=%llu) voice_drops=%u | "
           "audren_ticks=%llu dt[avg=%.2fms max=%.2fms >8ms=%llu] | modo=%s perf=%s\n",
           wall, (double)(prod - l_prod) / wall, (double)(uint32_t)(played - l_played) / wall,
           ((double)(uint32_t)(played - l_played) - (double)(prod - l_prod)) / wall,
           (unsigned long long)(prodb - l_prodb), (unsigned long long)(cons - l_cons),
           (unsigned long long)(underrun - l_underrun),
           omin == 0xFFFFFFFFu ? 0u : omin, omax, ocur,
           (unsigned long long)g_at.ring_overflow, (unsigned long long)(trim - l_trim),
           g_at.voice_drops.load(std::memory_order_relaxed),
           (unsigned long long)d_ticks,
           d_ticks ? (double)(dtsum - l_dtsum) / (double)d_ticks / 1000.0 : 0.0,
           (double)dtmax / 1000.0, (unsigned long long)(over8 - l_over8),
           opm == AppletOperationMode_Console ? "DOCK" : "HANDHELD",
           perf == ApmPerformanceMode_Boost ? "Boost" : "Normal");
    printf("[AudioLevels] raw_spu(post-clamp16): peak=%d clip>=32700=%llu/%llu | pre_limiter(mezcla final): peak=%d clip>=32700=%llu >%d=%llu | "
           "post_limiter: peak=%d clip>=32700=%llu | soft_limit calls=%llu activas(>=%d)=%llu (%.2f%%)\n",
           g_at.raw_peak, (unsigned long long)g_at.raw_clip, (unsigned long long)g_at.raw_samples,
           g_at.pre_peak, (unsigned long long)g_at.pre_clip, AUDIO_LIMITER_THRESHOLD, (unsigned long long)g_at.pre_over24576,
           g_at.post_peak, (unsigned long long)g_at.post_clip,
           (unsigned long long)(lcalls - l_lim_calls), AUDIO_LIMITER_THRESHOLD, (unsigned long long)(lact - l_lim_active),
           (lcalls - l_lim_calls) ? 100.0 * (double)(lact - l_lim_active) / (double)(lcalls - l_lim_calls) : 0.0);

    l_prod = prod; l_prodb = prodb; l_cons = cons; l_ticks = ticks; l_dtsum = dtsum; l_over8 = over8;
    l_played = played; l_underrun = underrun; l_lim_calls = lcalls; l_lim_active = lact; l_trim = trim;
    (void)l_ovf;
    g_at.raw_peak = g_at.pre_peak = g_at.post_peak = 0;
    g_at.raw_clip = g_at.raw_samples = g_at.pre_clip = g_at.pre_over24576 = g_at.post_clip = 0;
}

void SwitchAudio::AudioThreadEntry(void* arg) {
    SwitchAudio* self = static_cast<SwitchAudio*>(arg);
    alignas(64) int16_t last_block[BufferSamples] = {0};
    bool has_last_block = false;
    bool had_underrun = false;
    bool primed = false;
    float current_pitch = 1.0f;

    // Rolling ambient history buffer (32768 stereo frames = ~743 ms at 44100 Hz)
    // Runs 100% on Core 2 to generate seamless ambient continuity audio during synchronous file loads
    static constexpr size_t AmbientHistoryFrames = 32768;
    alignas(64) static int16_t s_ambient_history[AmbientHistoryFrames * 2] = {0};
    size_t ambient_write_pos = 0;
    float ambient_decay = 1.0f;
    float ambient_lpf_l = 0.0f;
    float ambient_lpf_r = 0.0f;
    size_t synth_tap1 = 0;
    size_t synth_tap2 = 16384;
    uint32_t s_noise_seed = 0x12345678;

    uint32_t health_tick_cnt = 0;
    uint32_t min_occ_win = RingSlots;
    uint32_t max_occ_win = 0;
    int16_t peak_l_win = 0;
    int16_t peak_r_win = 0;
    uint64_t clipped_samples_win = 0;
    uint64_t compressed_samples_win = 0;

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
        if (prev_tick) {
            uint32_t dt_us = (uint32_t)(armTicksToNs(t0 - prev_tick) / 1000ULL);
            g_at.dt_sum_us.fetch_add(dt_us, std::memory_order_relaxed);
            g_at.ticks.fetch_add(1, std::memory_order_relaxed);
            if (dt_us > 8000) g_at.dt_over8.fetch_add(1, std::memory_order_relaxed);
            if (dt_us > g_at.dt_max_us.load(std::memory_order_relaxed))
                g_at.dt_max_us.store(dt_us, std::memory_order_relaxed);
        }
        prev_tick = t0;

        audrvUpdate(&self->m_drv);
        g_at.played_samples.store(audrvVoiceGetPlayedSampleCount(&self->m_drv, 0), std::memory_order_relaxed);
        g_at.voice_drops.store(audrvVoiceGetVoiceDropsCount(&self->m_drv, 0), std::memory_order_relaxed);

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
        if (primed && occupancy < g_at.occ_min.load(std::memory_order_relaxed))
            g_at.occ_min.store(occupancy, std::memory_order_relaxed);
        if (occupancy > g_at.occ_max.load(std::memory_order_relaxed))
            g_at.occ_max.store(occupancy, std::memory_order_relaxed);
        g_at.occ_cur.store(occupancy, std::memory_order_relaxed);

#if 0
        if (++health_tick_cnt >= 200) {
            u32 voice_drops = audrvVoiceGetVoiceDropsCount(&self->m_drv, 0);
            printf("[AudioHealth] peak=(L:%d, R:%d) comp=%llu clip=%llu occ=[min=%u, max=%u] pitch=%.4f voice_drops=%u underruns=%llu\n",
                   peak_l_win, peak_r_win, (unsigned long long)compressed_samples_win,
                   (unsigned long long)clipped_samples_win,
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
            compressed_samples_win = 0;
        }
#endif

        // 1. Priming: acumular 2-3 bloques (~33-50 ms) para estabilidad total sin caídas a 0
        if (!primed) {
            uint32_t target_prime = has_last_block ? 2 : 3;
            if (occupancy >= target_prime) {
                primed = true;
            } else {
                continue;
            }
        }

        // 2. Control de latencia del búfer anular:
        // Mantener el anillo en 2-3 bloques (~33-50 ms): latencia óptima sin riesgo de underruns ni bocina barata.
        // Si tras una pausa o carga se acumulan bloques excesivos (> 5), recortamos el exceso a 3.
        if (occupancy > 5) {
            uint32_t drop_count = occupancy - 3;
            g_at.trim_drops.fetch_add(drop_count, std::memory_order_relaxed);
            r += drop_count;
            self->m_ringReadIdx.store(r, std::memory_order_release);
        }

        bool queuedAny = false;
        while (activeBuffers < 2) {
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
                    if (al > AUDIO_LIMITER_THRESHOLD || ar > AUDIO_LIMITER_THRESHOLD) compressed_samples_win++;
                    if (al >= 32700 || ar >= 32700) clipped_samples_win++;

                    // Capture outgoing samples into rolling ambient history
                    s_ambient_history[ambient_write_pos * 2]     = sl;
                    s_ambient_history[ambient_write_pos * 2 + 1] = sr;
                    ambient_write_pos = (ambient_write_pos + 1) & (AmbientHistoryFrames - 1);
                }

                // Reset ambient synthesis parameters for whenever an underrun might occur
                ambient_decay = 1.0f;
                synth_tap1 = (ambient_write_pos + AmbientHistoryFrames - 22050) & (AmbientHistoryFrames - 1);
                synth_tap2 = (ambient_write_pos + AmbientHistoryFrames - 11025) & (AmbientHistoryFrames - 1);

                if (had_underrun && has_last_block) {
                    // 64-sample ramp-in crossfade from ambient synthesis to eliminate resumption click
                    int16_t* dst = self->m_audioBuffers[self->m_currentBuffer];
                    for (size_t s = 0; s < 128 && s < BufferSamples; s += 2) {
                        int32_t blend = (int32_t)(s / 2); // 0 to 63
                        int16_t cur_l = s_ptr[s];
                        int16_t cur_r = s_ptr[s + 1];
                        int16_t prev_l = last_block[s];
                        int16_t prev_r = last_block[s + 1];
                        dst[s]     = (int16_t)((prev_l * (64 - blend) + cur_l * blend) / 64);
                        dst[s + 1] = (int16_t)((prev_r * (64 - blend) + cur_r * blend) / 64);
                    }
                    if (BufferSamples > 128) {
                        memcpy(dst + 128, s_ptr + 128, BufferSizeBytes - 128 * sizeof(int16_t));
                    }
                    had_underrun = false;
                } else {
                    memcpy(self->m_audioBuffers[self->m_currentBuffer], s_ptr, BufferSizeBytes);
                }
                memcpy(last_block, s_ptr, BufferSizeBytes);
                has_last_block = true;
                self->m_ringReadIdx.store(r + 1, std::memory_order_release);
                g_at.consumed_blocks.fetch_add(1, std::memory_order_relaxed);
            } else {
                g_prof_audio_underruns++;
                had_underrun = true;
                if (occupancy == 0) {
                    primed = false;
                }

                // Independent ambient continuity synthesis on Core 2:
                // Instead of abruptly decaying into dead silence during synchronous file loads,
                // generate a warm, diffused ambient reverb tail from recent audio history.
                int16_t* dst = self->m_audioBuffers[self->m_currentBuffer];

                for (size_t s = 0; s < BufferSamples; s += 2) {
                    int16_t s1_l = s_ambient_history[synth_tap1 * 2];
                    int16_t s1_r = s_ambient_history[synth_tap1 * 2 + 1];
                    int16_t s2_l = s_ambient_history[synth_tap2 * 2];
                    int16_t s2_r = s_ambient_history[synth_tap2 * 2 + 1];

                    synth_tap1 = (synth_tap1 + 1) & (AmbientHistoryFrames - 1);
                    synth_tap2 = (synth_tap2 + 1) & (AmbientHistoryFrames - 1);

                    // Diffused blend of both taps
                    float in_l = (float)(s1_l + s2_l) * 0.5f * ambient_decay;
                    float in_r = (float)(s1_r + s2_r) * 0.5f * ambient_decay;

                    // Gentle high-cut low-pass filter (~3.5 kHz) for a warm, natural room reverb tail
                    ambient_lpf_l += 0.35f * (in_l - ambient_lpf_l);
                    ambient_lpf_r += 0.35f * (in_r - ambient_lpf_r);

                    // Subtle analog warm room presence (-54 dBFS pinkish dither)
                    s_noise_seed = s_noise_seed * 1664525u + 1013904223u;
                    float dither = ((float)((int32_t)s_noise_seed >> 24)) * 0.15f;

                    int32_t out_l = (int32_t)(ambient_lpf_l + dither);
                    int32_t out_r = (int32_t)(ambient_lpf_r + dither);

                    if (out_l > 32767) out_l = 32767; else if (out_l < -32768) out_l = -32768;
                    if (out_r > 32767) out_r = 32767; else if (out_r < -32768) out_r = -32768;

                    dst[s]         = (int16_t)out_l;
                    dst[s + 1]     = (int16_t)out_r;
                    last_block[s]     = (int16_t)out_l;
                    last_block[s + 1] = (int16_t)out_r;
                }
                has_last_block = true;

                // Graceful natural room decay: 0.985 per 16.67ms buffer (~63% volume over 500ms)
                ambient_decay *= 0.985f;
                if (ambient_decay < 0.05f) ambient_decay = 0.05f;
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

static u64 s_last_audio_update_tick = 0;
static u64 s_last_vblank_audio_tick = 0;
static std::atomic<bool> s_in_audio_update{false};

void SwitchAudio::Update() {
    if (!m_initialized) return;

    bool expected = false;
    if (!s_in_audio_update.compare_exchange_strong(expected, true)) {
        return;
    }
    struct UpdateGuard {
        ~UpdateGuard() {
            s_in_audio_update.store(false, std::memory_order_release);
        }
    } guard;

    u64 now_tick = armGetSystemTick();
    uint64_t vblank_elapsed_ns = (s_last_vblank_audio_tick != 0) ? armTicksToNs(now_tick - s_last_vblank_audio_tick) : 0;
    s_last_vblank_audio_tick = now_tick;

    int blocks_to_render = 1;
    if (m_threadCreated) {
        uint32_t cur_w = m_ringWriteIdx.load(std::memory_order_relaxed);
        uint32_t cur_r = m_ringReadIdx.load(std::memory_order_relaxed);
        uint32_t cur_occ = (cur_w >= cur_r) ? (cur_w - cur_r) : 0;
        if (cur_occ <= 1) {
            // Si el anillo cae a 1 o 0, generamos 2 bloques de inmediato para reponer el colchon a 2-3
            blocks_to_render = 2;
        } else if (cur_occ >= 4) {
            // Si el anillo sube a >= 4 bloques (~66 ms), no renderizamos en este VBlank para drenar a 2-3
            s_last_audio_update_tick = now_tick;
            blocks_to_render = 0;
        } else {
            blocks_to_render = 1;
        }
    }

    for (int b = 0; b < blocks_to_render; b++) {
        u64 t_spu0 = armGetSystemTick();
        s_last_audio_update_tick = t_spu0;
        alignas(64) int16_t tempPcm[BufferSamples];
        spu_render(tempPcm, FramesPerStep);
        telem_scan(tempPcm, BufferSamples, g_at.post_peak, g_at.post_clip, &g_at.pre_over24576);
        g_at.pre_peak = g_at.post_peak;
        g_at.pre_clip = g_at.post_clip;
        g_at.produced_frames += FramesPerStep;
        g_at.produced_blocks++;
        u64 t_spu1 = armGetSystemTick();
        uint64_t spu_ns = armTicksToNs(t_spu1 - t_spu0);
        g_prof_audio_spu_ns += spu_ns;

        if (m_threadCreated) {
            // Encolar en el anillo lock-free sin bloqueos
            uint32_t w = m_ringWriteIdx.load(std::memory_order_relaxed);
            uint32_t r = m_ringReadIdx.load(std::memory_order_acquire);

            if (w - r >= (uint32_t)RingSlots) {
                m_ringReadIdx.store(r + 1, std::memory_order_release);
                g_at.ring_overflow++;
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

void switch_audio_pump_if_needed(void) {
    if (!SwitchAudio::Instance().IsInitialized()) return;
    if (s_in_audio_update.load(std::memory_order_relaxed)) return;
    u64 now = armGetSystemTick();
    if (s_last_audio_update_tick == 0) {
        s_last_audio_update_tick = now;
        return;
    }
    // 250.000 ticks = ~13.0 ms (actúa si un frame o lectura de SD se bloquea más de ~13 ms)
    constexpr u64 TICKS_PUMP_THRESHOLD = 250000;
    if (now - s_last_audio_update_tick >= TICKS_PUMP_THRESHOLD) {
        SwitchAudio::Instance().Update();
    }
}

void switch_audio_queue(const int16_t* pcm_stereo, size_t frames) {
    SwitchAudio::Instance().QueueFrames(pcm_stereo, frames);
}

void switch_audio_shutdown(void) {
    SwitchAudio::Instance().Shutdown();
}

}
