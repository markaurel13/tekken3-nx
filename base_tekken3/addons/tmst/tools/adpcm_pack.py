"""The Jukebox's ADPCM packs (src/jukebox/tmst_adpcm.h): a .bgm that keeps the
game's own PS-ADPCM, so it takes about a quarter of the decoded frames' room,
and the Jukebox decodes it as it plays (tools/ttt_ps2_music.py,
tekken5dr_music.py write them).

'T3ADP001', u32 rate (44 100), u32 frames, u32 loop frame (0: the
beginning), u32 gain (per 65 536, on every decoded sample), u32 0, u32 0; then
per block 16 bytes of the left channel's PS-ADPCM and 16 of the right (28
frames). The game's bytes as they are: nothing is lost.

decode() is the Jukebox's decoding, sample for sample (tests/test_adpcm_pack.py
checks the C against it)."""
from __future__ import annotations

import struct
from array import array
from pathlib import Path

MAGIC, HEAD, BLOCK, FRAMES, RATE = b'T3ADP001', 32, 16, 28, 44100
F0, F1 = (0, 60, 115, 98, 122), (0, 0, -52, -55, -60)


def gain_of(gain: float) -> int:
    """A gain as the header keeps it (per 65 536)."""
    return max(0, min(0xFFFFFFFF, round(gain * 65536)))


def write(path, left: bytes, right: bytes, loop: int, gain: float) -> int:
    """A pack of the two channels' PS-ADPCM (whole blocks; the shorter channel
    decides), looping at frame loop (0 when not inside); its frames."""
    blocks = min(len(left), len(right)) // BLOCK
    frames = blocks * FRAMES
    if not blocks:
        raise ValueError('no ADPCM blocks')
    if not 0 <= loop < frames:
        loop = 0
    body = bytearray(2 * BLOCK * blocks)
    for b in range(blocks):
        at = b * BLOCK
        body[2 * at:2 * at + BLOCK] = left[at:at + BLOCK]
        body[2 * at + BLOCK:2 * at + 2 * BLOCK] = right[at:at + BLOCK]
    path = Path(path)
    temp = path.with_name(path.name + '.new')
    temp.write_bytes(MAGIC + struct.pack('<5I', RATE, frames, loop, gain_of(gain), 0) + bytes(4) + body)
    temp.replace(path)
    return frames


def header(data: bytes) -> tuple[int, int, int]:
    """(frames, loop, gain) of a pack."""
    if data[:8] != MAGIC:
        raise ValueError('not an ADPCM pack')
    rate, frames, loop, gain = struct.unpack_from('<4I', data, 8)
    return frames, loop, gain


def decode(data: bytes) -> array:
    """A pack's frames from its beginning to its end (s16, left and right)."""
    frames, loop, gain = header(data)
    out = array('h', bytes(4 * frames))
    nibbles = [[(((n << 12) ^ 0x8000) - 0x8000) >> s for n in range(16)] for s in range(13)]
    for ch in (0, 1):
        h0 = h1 = 0
        i = ch
        for at in range(HEAD + ch * BLOCK, len(data) - BLOCK + 1, 2 * BLOCK):
            b = data[at]
            shift, filt = b & 15, b >> 4
            nib = nibbles[shift if shift <= 12 else 9]
            f0, f1 = F0[filt if filt <= 4 else 4], F1[filt if filt <= 4 else 4]
            for byte in data[at + 2:at + BLOCK]:
                for n in (byte & 15, byte >> 4):
                    v = nib[n] + ((h0 * f0 + h1 * f1 + 32) >> 6)
                    v = 32767 if v > 32767 else -32768 if v < -32768 else v
                    h1, h0 = h0, v
                    if i < 2 * frames:
                        s = (v * gain) >> 16
                        out[i] = 32767 if s > 32767 else -32768 if s < -32768 else s
                    i += 2
    return out


# ---- XA packs (src/jukebox/tmst_adpcm.h): a disc's XA audio as it is ---------------------------
XA_MAGIC, XA_RATE, XA_GROUP, XA_GROUP_FRAMES = b'T3XA0001', 37800, 128, 112


def write_xa(path, groups: bytes, gain: float) -> int:
    """A pack of stereo 37 800 Hz XA sound groups (128 bytes each), at gain
    after the Jukebox's resampling; its frames at 37 800 Hz."""
    count = len(groups) // XA_GROUP
    if not count or len(groups) % XA_GROUP:
        raise ValueError('not whole XA sound groups')
    frames = count * XA_GROUP_FRAMES
    path = Path(path)
    temp = path.with_name(path.name + '.new')
    temp.write_bytes(XA_MAGIC + struct.pack('<5I', XA_RATE, frames, 0, gain_of(gain), 0) + bytes(4) + groups)
    temp.replace(path)
    return frames


def xa_header(data: bytes) -> tuple[int, int]:
    """(frames at 37 800 Hz, gain) of an XA pack."""
    if data[:8] != XA_MAGIC:
        raise ValueError('not an XA pack')
    rate, frames, loop, gain = struct.unpack_from('<4I', data, 8)
    return frames, gain
