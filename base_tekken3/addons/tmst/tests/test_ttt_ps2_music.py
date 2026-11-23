"""tools/ttt_ps2_music.py, on a small pack of our own."""
import hashlib, json, struct, sys, tempfile, unittest
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ADDON / 'tools'))           # by name: its worker processes import it again
import ttt_ps2_music  # noqa: E402
import adpcm_pack  # noqa: E402


def sector(left: int, right: int) -> bytes:
    """Every block: filter 0, shift 12, all nibbles the same."""
    return (bytes([12, 0]) + bytes([left * 17]) * 14) * 64 + (bytes([12, 0]) + bytes([right * 17]) * 14) * 64


class TttPs2MusicTest(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        head = b'TTP2' + struct.pack('<II', 1, 1)
        offset = len(head) + 20
        head += struct.pack('<5I', 0x09, offset, 2, 1, 0x3FFF)
        self.pack = self.root / ttt_ps2_music.PACKS[0]
        self.pack.parent.mkdir(parents=True)
        self.pack.write_bytes(head + sector(1, 15) * 2)

    def tearDown(self) -> None:
        self.tmp.cleanup()

    def test_pack_to_bgm(self) -> None:
        self.assertEqual(ttt_ps2_music.find_pack(self.root), self.pack)
        out = self.root / ttt_ps2_music.OUT
        self.assertEqual(ttt_ps2_music.convert(self.pack, out, say=lambda _: None), ['ps2-paul.bgm'])
        data = (out / 'ps2-paul.bgm').read_bytes()
        frames = 2 * ttt_ps2_music.SECTOR_FRAMES
        self.assertEqual(adpcm_pack.header(data), (frames, ttt_ps2_music.SECTOR_FRAMES, 65536))
        self.assertEqual(len(data), adpcm_pack.HEAD + frames // 28 * 32)   # the sectors' bytes, as they are
        self.assertEqual(tuple(adpcm_pack.decode(data)[:2]), (1, -1))     # nibble 1 >> 12, nibble 15 (-1) >> 12

    def test_disc_extras(self) -> None:
        disc = self.root / 'ttt.iso'
        disc.write_bytes(bytes(2048 * 3) + sector(2, 14) * 2)              # the piece at LBA 3
        (self.root / '.setup').mkdir()
        (self.root / '.setup/last-inputs.json').write_text(json.dumps({'ttt_disc': str(disc)}), encoding='utf-8')
        self.assertEqual(ttt_ps2_music.find_disc(self.root), disc)
        good = (0x01, 'embu', 3, 2, 1, 0x3FFF, hashlib.sha256(sector(2, 14) * 2).hexdigest())
        bad = (0x02, 'bgm-02', 3, 2, 0, 0x3FFF, '0' * 64)
        extras, ttt_ps2_music.EXTRAS = ttt_ps2_music.EXTRAS, [good, bad]
        try:
            out = self.root / ttt_ps2_music.OUT
            names = ttt_ps2_music.convert(None, out, disc, say=lambda _: None, skip=())
        finally:
            ttt_ps2_music.EXTRAS = extras
        self.assertEqual(names, ['ps2-embu.bgm'])                          # the other's sectors are not the expected
        data = (out / 'ps2-embu.bgm').read_bytes()
        self.assertEqual(adpcm_pack.header(data)[:2], (2 * ttt_ps2_music.SECTOR_FRAMES, ttt_ps2_music.SECTOR_FRAMES))
        self.assertEqual(tuple(adpcm_pack.decode(data)[:2]), (2, -2))

    def test_skipped_not_made(self) -> None:
        out = self.root / ttt_ps2_music.OUT
        out.mkdir(parents=True)
        (out / 'ps2-bgm-02.bgm').write_bytes(b'T3BGM001')             # made before: removed
        self.assertEqual(ttt_ps2_music.convert(self.pack, out, say=lambda _: None, skip=['ps2-paul.bgm', 'ps2-bgm-02.bgm']), [])
        self.assertEqual(list(out.iterdir()), [])
        self.assertEqual(ttt_ps2_music.skipped(), ['ps2-bgm-02.bgm', 'ps2-bgm-03.bgm', 'ps2-bgm-08.bgm'])

    def test_no_pack(self) -> None:
        self.pack.unlink()
        self.assertIsNone(ttt_ps2_music.find_pack(self.root))


if __name__ == '__main__':
    unittest.main()
