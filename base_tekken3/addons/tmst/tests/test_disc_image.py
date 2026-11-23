"""tools/disc_image.py and tools/tekken2_disc.py on a made-up disc (no game
data): an ISO 9660 track 1 with only a SYSTEM.CNF, and a silent audio track."""
from __future__ import annotations
import struct, sys, tempfile, unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import disc_image, tekken2_disc  # noqa: E402


def record(name: bytes, lba: int, size: int, folder: bool) -> bytes:
    body = b'\0' + struct.pack('<I', lba) + struct.pack('>I', lba) + struct.pack('<I', size) + struct.pack('>I', size)
    body += bytes(7) + bytes([2 if folder else 0, 0, 0]) + b'\x01\x00\x00\x01' + bytes([len(name)]) + name
    if len(body) % 2 == 0:
        body += b'\0'
    return bytes([len(body) + 1]) + body


def mode2(user: bytes) -> bytes:
    return disc_image.SYNC + b'\0\0\0\x02' + bytes(8) + user.ljust(2048, b'\0') + bytes(280)


def track1(boot: str) -> bytes:
    cnf = f'BOOT = cdrom:\\{boot};1\r\nTCB = 4\r\n'.encode()
    pvd = bytearray(2048)
    pvd[0:6] = b'\x01CD001'
    pvd[156:156 + 34] = record(b'\0', 18, 2048, True)
    root = record(b'\0', 18, 2048, True) + record(b'\1', 18, 2048, True) + record(b'SYSTEM.CNF;1', 20, len(cnf), False)
    sectors = [b''] * 16 + [bytes(pvd), b'\xffCD001', root, b'', cnf] + [b''] * 4
    return b''.join(mode2(s) for s in sectors)


class DiscImageTest(unittest.TestCase):
    def setUp(self):
        self.dir = Path(tempfile.mkdtemp())

    def make(self, boot: str, single: bool) -> Path:
        data, audio = track1(boot), bytes(2352 * 10)
        if single:
            (self.dir / 'Tekken 2 (USA).bin').write_bytes(data + audio)
            cue = ('FILE "Tekken 2 (USA).bin" BINARY\n  TRACK 01 MODE2/2352\n    INDEX 01 00:00:00\n'
                   f'  TRACK 02 AUDIO\n    INDEX 00 00:00:{len(data) // 2352:02d}\n    INDEX 01 00:00:{len(data) // 2352 + 2:02d}\n')
        else:
            (self.dir / 'Tekken 2 (USA) (Track 1).bin').write_bytes(data)
            (self.dir / 'Tekken 2 (USA) (Track 2).bin').write_bytes(audio)
            cue = ('FILE "Tekken 2 (USA) (Track 1).bin" BINARY\n  TRACK 01 MODE2/2352\n    INDEX 01 00:00:00\n'
                   'FILE "Tekken 2 (USA) (Track 2).bin" BINARY\n  TRACK 02 AUDIO\n    INDEX 00 00:00:00\n'
                   '    INDEX 01 00:02:00\n')
        (self.dir / 'Tekken 2 (USA).cue').write_text(cue)
        return self.dir / 'Tekken 2 (USA).cue'

    def test_split_cue_and_its_bin(self):
        cue = self.make('SLUS_002.13', single=False)
        for path in (cue, self.dir / 'Tekken 2 (USA) (Track 1).bin'):
            info = tekken2_disc.identify(path)
            self.assertEqual((info['region'], info['serial'], info['tracks']), ('USA', 'SLUS-00213', 2))

    def test_single_file_dump(self):
        cue = self.make('SCES_002.55', single=True)
        with disc_image.open_disc(cue) as disc:
            self.assertEqual([t.kind for t in disc.tracks], ['MODE2/2352', 'AUDIO'])
            self.assertEqual(disc.tracks[0].frames, 25)
        self.assertEqual(tekken2_disc.identify(cue)['region'], 'Europe')

    def test_another_game_is_refused(self):
        cue = self.make('SLUS_004.02', single=False)
        with self.assertRaisesRegex(disc_image.DiscError, 'not Tekken 2'):
            tekken2_disc.identify(cue)

    def test_search_prefers_the_cue(self):
        self.make('SLUS_002.13', single=False)
        self.assertEqual([p.name for p in tekken2_disc.search([self.dir])], ['Tekken 2 (USA).cue'])

    def test_audio_track_from_its_index_01(self):
        cue = self.make('SLUS_002.13', single=True)
        with disc_image.open_disc(cue) as disc:
            self.assertEqual(len(disc.audio(2)), 8 * 2352)                  # 10 sectors, 2 of them the pregap
            with self.assertRaises(disc_image.DiscError):
                disc.audio(3)

    def test_flac_frames(self):
        """Two frames of our own: a fixed-order-2 subframe (Rice coded, one partition
        escaped) beside a constant one, then left/side verbatim and a wasted bit."""
        bits = []

        def put(value, n):
            bits.extend((value >> (n - 1 - i)) & 1 for i in range(n))

        def rice(v, k):
            u = v * 2 if v >= 0 else -v * 2 - 1
            put(0, u >> k); put(1, 1); put(u & ((1 << k) - 1), k)

        def header(n, chan):
            put(0x7FFC, 15); put(0, 1); put(7, 4); put(9, 4); put(chan, 4); put(4, 3); put(0, 1)
            put(0, 8); put(n - 1, 16); put(0, 8)                           # frame 0, block size, CRC-8

        def footer():
            put(0, -len(bits) % 8); put(0, 16)                             # byte boundary, CRC-16

        left = [10, 12, 15, 17, 16, 14, 11, 9]
        header(8, 1)
        put(0, 1); put(8 + 2, 6); put(0, 1); put(10, 16); put(12, 16)      # fixed order 2: warm-up
        put(0, 2); put(1, 4)                                               # Rice, 2 partitions
        put(2, 4)
        for i in (2, 3):
            rice(left[i] - (2 * left[i - 1] - left[i - 2]), 2)
        put(15, 4); put(4, 5)                                              # escaped: 4-bit samples
        for i in range(4, 8):
            put((left[i] - (2 * left[i - 1] - left[i - 2])) & 15, 4)
        put(0, 1); put(0, 6); put(0, 1); put(-300 & 0xFFFF, 16)            # constant -300
        footer()
        header(2, 8)                                                       # left, side
        put(0, 1); put(1, 6); put(1, 1); put(1, 1); put(500 >> 1, 15); put(-4 >> 1 & 0x7FFF, 15)
        put(0, 1); put(1, 6); put(0, 1); put(100, 17); put(-200 & 0x1FFFF, 17)
        footer()
        data = bytes(int(''.join(map(str, bits[i:i + 8])), 2) for i in range(0, len(bits), 8)) + b'rest'
        pcm, used = disc_image.flac_decode(data, 10)
        self.assertEqual(list(pcm[0::2]), left + [500, -4])
        self.assertEqual(list(pcm[1::2]), [-300] * 8 + [400, 196])
        self.assertEqual(data[used:], b'rest')

    def test_bits_past_the_end_are_zero(self):
        bits = disc_image.Bits(b'\xa5')
        self.assertEqual((bits.read(4), bits.peek(8), bits.read(6)), (0xA, 0x50, 0x14))


if __name__ == '__main__':
    unittest.main()
