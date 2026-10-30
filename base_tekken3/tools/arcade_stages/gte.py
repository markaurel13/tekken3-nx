"""Arcade Stages E2: the few GTE operations of the stage drawing, exact to the bit
(RTPS/RTPT with sf = 1, lm = 0, and NCLIP), after the documented PlayStation
GTE (unsigned Newton-Raphson division). Vectorised with numpy.

A camera is the GTE state at the end of the tile drawing (0x801D99C0), as
capture.py --dump logs it: rotation (3 x 3 s16, 4.12), translation (s32),
OFX, OFY (s32, 16.16) and H (u16).
"""
from __future__ import annotations

from dataclasses import dataclass

import numpy as np

UNR = np.array([max(0, (0x40000 // (i + 0x100) + 1) // 2 - 0x101) for i in range(0x101)], np.int64)


@dataclass
class Camera:
    rot: np.ndarray       # (3, 3) int
    trans: np.ndarray     # (3,) int
    ofx: int
    ofy: int
    h: int

    @classmethod
    def from_gte(cls, r11r12, r13r21, r22r23, r31r32, r33, trx, try_, trz, ofx, ofy, h, **_):
        s16 = lambda v: (v & 0xFFFF) - ((v & 0x8000) << 1)
        s32 = lambda v: (v & 0xFFFFFFFF) - ((v & 0x80000000) << 1)
        rot = np.array([[s16(r11r12), s16(r11r12 >> 16), s16(r13r21)],
                        [s16(r13r21 >> 16), s16(r22r23), s16(r22r23 >> 16)],
                        [s16(r31r32), s16(r31r32 >> 16), s16(r33)]], np.int64)
        return cls(rot, np.array([s32(trx), s32(try_), s32(trz)], np.int64), s32(ofx), s32(ofy), h & 0xFFFF)

    def to_json(self):
        return dict(rot=self.rot.tolist(), trans=self.trans.tolist(), ofx=self.ofx, ofy=self.ofy, h=self.h)

    @classmethod
    def from_json(cls, d):
        return cls(np.array(d['rot'], np.int64), np.array(d['trans'], np.int64), d['ofx'], d['ofy'], d['h'])


def _clz16(v):
    """Leading zeros of v as a 16-bit number (v > 0)."""
    n = np.zeros_like(v)
    for b in range(15, -1, -1):
        n = np.where((v >> b) == 0, 16 - b, n)
    return np.where(v == 0, 16, np.minimum(n, 16))


def _divide(h, sz3):
    """H / SZ3 as the GTE does it, 17 bits (0x1FFFF on overflow)."""
    ok = h < sz3 * 2
    sz = np.where(ok, sz3, 1)
    z = np.minimum(_clz16(sz), 15)
    n = np.int64(h) << z
    d = sz << z
    u = UNR[(d - 0x7FC0) >> 7] + 0x101
    d = (0x2000080 - d * u) >> 8
    d = (0x0000080 + d * u) >> 8
    q = np.minimum(0x1FFFF, (n * d + 0x8000) >> 16)
    return np.where(ok, q, 0x1FFFF)


def rtps(cam: Camera, v: np.ndarray):
    """v: (N, 3) s16 -> (sx, sy, sz) int arrays, SXY saturated to -0x400..0x3FF."""
    v = np.asarray(v, np.int64)
    mac = (cam.trans[None, :] << 12) + v @ cam.rot.T       # (N, 3), 44-bit, no overflow here
    mac = mac >> 12                                         # sf = 1
    ir = np.clip(mac, -0x8000, 0x7FFF)
    sz3 = np.clip(mac[:, 2], 0, 0xFFFF)
    q = _divide(cam.h, sz3)
    sx = np.clip((q * ir[:, 0] + cam.ofx) >> 16, -0x400, 0x3FF)
    sy = np.clip((q * ir[:, 1] + cam.ofy) >> 16, -0x400, 0x3FF)
    return sx, sy, sz3


def nclip(x0, y0, x1, y1, x2, y2):
    return x0 * y1 + x1 * y2 + x2 * y0 - x0 * y2 - x1 * y0 - x2 * y1
