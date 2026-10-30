"""Minimal PINE client for PCSX2 (from the devil-jin branch, tools/t5/pine.py) (Settings > Advanced > PINE, or
EnablePINE = true in inis/PCSX2.ini): the Unix socket $TMPDIR/pcsx2.sock on
macOS / Linux, TCP 127.0.0.1:<slot> on Windows.

One connection is kept open: PCSX2 logs every connection, and opening one
per request filled its log with hundreds of megabytes and slowed the game.
"""
import glob, os, re, socket, struct
from pathlib import Path

OP = dict(r8=0, r16=1, r32=2, r64=3, w8=4, w16=5, w32=6, w64=7, version=8, save=9, load=10, title=11, status=15)


def ttt_slot():
    """PINE slot of this worktree's TTT instance: PINESlot in its own
    workspace/ttt-ps2/pcsx2-data/PCSX2/inis/PCSX2.ini (each worktree's copy has
    its own, 28012 for the Embu worktree, 28013 for the capture one), unless
    PCSX2_PINE_SLOT is set."""
    if os.environ.get('PCSX2_PINE_SLOT'): return int(os.environ['PCSX2_PINE_SLOT'])
    ini = Path(__file__).resolve().parents[2] / 'workspace/ttt-ps2/pcsx2-data/PCSX2/inis/PCSX2.ini'
    m = ini.exists() and re.search(r'^PINESlot\s*=\s*(\d+)', ini.read_text(errors='replace'), re.M)
    return int(m.group(1)) if m else 28012


class Pine:
    def __init__(self, slot=None):
        # Slot 28011 is PCSX2's default socket, pcsx2.sock; any other slot
        # (a second instance with its own -datapath) is pcsx2.sock.<slot>.
        slot = int(slot or os.environ.get('PCSX2_PINE_SLOT', 28011))
        self.sock = None
        if os.name == 'nt':                         # Windows: PINE listens on TCP, port = slot
            self.path = ('127.0.0.1', slot)
            return
        name = 'pcsx2.sock' if slot == 28011 else f'pcsx2.sock.{slot}'
        candidates = [os.path.join(os.environ.get('TMPDIR', '/tmp'), name), '/tmp/' + name]
        candidates += glob.glob('/var/folders/*/*/T/' + name)
        self.path = next((p for p in candidates if os.path.exists(p)), None)
        if not self.path: raise FileNotFoundError('PCSX2 PINE socket not found; is PCSX2 running with PINE on?')

    def call(self, payload):
        if self.sock is None:
            if isinstance(self.path, tuple):
                self.sock = socket.create_connection(self.path)
                self.sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
            else:
                self.sock = socket.socket(socket.AF_UNIX); self.sock.connect(self.path)
        self.sock.sendall(struct.pack('<I', len(payload) + 4) + payload)
        head = b''
        while len(head) < 4: head += self.sock.recv(4 - len(head))
        n = struct.unpack('<I', head)[0]
        data = b''
        while len(data) < n - 4: data += self.sock.recv(n - 4 - len(data))
        if data[0] != 0: raise RuntimeError(f'PINE error {data[0]}')
        return data[1:]

    def wait(self, timeout=120):
        """Wait for PCSX2 to answer (launched at the same time: PINE opens
        once the game is booted)."""
        import time
        t0 = time.monotonic()
        while True:
            try: return self.title()
            except (ConnectionError, OSError, RuntimeError):   # RuntimeError: PINE 255 while the game boots
                self.sock = None
                if time.monotonic() - t0 > timeout: raise
                time.sleep(1)

    def r16(self, a): return struct.unpack('<H', self.call(bytes([OP['r16']]) + struct.pack('<I', a)))[0]
    def r32(self, a): return struct.unpack('<I', self.call(bytes([OP['r32']]) + struct.pack('<I', a)))[0]
    def r64(self, a): return struct.unpack('<Q', self.call(bytes([OP['r64']]) + struct.pack('<I', a)))[0]
    def w16(self, a, v): self.call(bytes([OP['w16']]) + struct.pack('<IH', a, v & 0xffff))
    def w32(self, a, v): self.call(bytes([OP['w32']]) + struct.pack('<II', a, v & 0xffffffff))
    def many(self, addrs, chunk=4000):
        """64-bit words at addrs, several PINE commands per message (one round trip per chunk)."""
        out = []
        for i in range(0, len(addrs), chunk):
            part = addrs[i:i + chunk]
            data = self.call(b''.join(bytes([OP['r64']]) + struct.pack('<I', a) for a in part))
            out += struct.unpack(f'<{len(part)}Q', data[:8 * len(part)])
        return out
    def block(self, a, n):
        """n bytes (a multiple of 8) from a, with 64-bit reads."""
        return b''.join(struct.pack('<Q', self.r64(a + k)) for k in range(0, n, 8))
    def title(self):
        d = self.call(bytes([OP['title']])); return d[4:4 + struct.unpack('<I', d[:4])[0]].decode('latin1')
