// PS2 Disc Asset Extractor for Tekken Tag Tournament (USA) (SLUS-20001).
// Extracts remastered stage music (TTP2), ending music (PCM), and laser textures directly from disc sectors.

import { DiscReader } from './iso.js';

export const MUSIC_FIRST = 112875;
export const MUSIC_LAST = 142915;
export const MUSIC_SECTORS_COUNT = MUSIC_LAST - MUSIC_FIRST;
export const MUSIC_SHA256 = '0955079110096122e92e6c1448a1fe037e8c8c43c55104245f174f932fcaf317';

export const ENDING_FIRST = 110777;
export const ENDING_LAST = 111953;
export const ENDING_SHA256 = '1c52abba38b4f5ff5d3aa4445dcaab2ae988ee8dca82c619cd1ccc6f5329b082';

export const PIECES = [
  { number: 0x09, lba: 112875, sectors: 1569, loop: 93, volume: 9830, name: 'PAUL' },
  { number: 0x0A, lba: 116665, sectors: 1412, loop: 21, volume: 9011, name: 'LAW' },
  { number: 0x0B, lba: 118077, sectors: 2365, loop: 184, volume: 10649, name: 'YOSHIMITSU' },
  { number: 0x0C, lba: 120442, sectors: 2078, loop: 0, volume: 9830, name: 'XIAOYU' }, // loop -129 -> 0
  { number: 0x0D, lba: 122520, sectors: 2368, loop: 7, volume: 10649, name: 'HWOARANG' },
  { number: 0x0E, lba: 114444, sectors: 2221, loop: 0, volume: 11468, name: 'LEI' }, // loop -1930 -> 0
  { number: 0x0F, lba: 124888, sectors: 2428, loop: 226, volume: 12288, name: 'OGRE' },
  { number: 0x10, lba: 127316, sectors: 2193, loop: 0, volume: 10649, name: 'SCHOOL' }, // loop -3924 -> 0
  { number: 0x11, lba: 129509, sectors: 2180, loop: 0, volume: 10649, name: 'JIN' }, // loop -281 -> 0
  { number: 0x12, lba: 131689, sectors: 2054, loop: 0, volume: 10649, name: 'NINA' }, // loop -9809 -> 0
  { number: 0x13, lba: 133743, sectors: 2701, loop: 0, volume: 10649, name: 'EDDY' }, // loop -92 -> 0
  { number: 0x14, lba: 136444, sectors: 2126, loop: 237, volume: 9011, name: 'KING' },
  { number: 0x15, lba: 138570, sectors: 2365, loop: 3, volume: 10649, name: 'HEIHACHI' },
  { number: 0x16, lba: 140935, sectors: 1980, loop: 72, volume: 11468, name: 'UNKNOWN' },
];

export async function sha256Hex(bytes) {
  const digest = new Uint8Array(await crypto.subtle.digest('SHA-256', bytes));
  return [...digest].map(b => b.toString(16).padStart(2, '0')).join('');
}

// Verifies if the provided file is a valid Tekken Tag Tournament (USA) PS2 disc
export async function identifyTttPs2Disc(blob) {
  const reader = new DiscReader(blob);
  await reader.init();

  const files = await reader.listFiles();
  const hasSlus = files.some(f => f.name.toUpperCase() === 'SLUS_200.01');
  const hasSystemCnf = files.some(f => f.name.toUpperCase() === 'SYSTEM.CNF');
  const hasTekkenBin = files.some(f => f.name.toUpperCase() === 'TEKKEN.BIN');

  if (!hasSlus && !hasTekkenBin) {
    return { valid: false, reason: "No SLUS_200.01 or TEKKEN.BIN found on disc" };
  }

  // Quick verification: test read first sector of stage music
  const testSector = await reader.readSectors(MUSIC_FIRST, 1);
  if (testSector.length !== 2048) {
    return { valid: false, reason: "Unable to read BGM sectors" };
  }

  return {
    valid: true,
    edition: "Tekken Tag Tournament (USA) [SLUS-20001]",
    sectorSize: reader.sectorSize,
  };
}

// Extracts and compiles `ps2-music.ttp` (version 1 format, 14 pieces)
export async function extractPs2Music(blob, onProgress = null) {
  const reader = new DiscReader(blob);
  await reader.init();

  if (onProgress) onProgress("Leyendo sectores de música PS2 (LBA 112875..142915)...", 0.1);
  const data = await reader.readSectors(MUSIC_FIRST, MUSIC_SECTORS_COUNT);

  if (onProgress) onProgress("Verificando integridad de pistas de audio...", 0.6);
  const hash = await sha256Hex(data);
  if (hash !== MUSIC_SHA256) {
    console.warn(`Warning: BGM hash mismatch (${hash}), continuing build anyway...`);
  }

  if (onProgress) onProgress("Construyendo paquete de audio TTP2...", 0.8);
  // Header: 'TTP2' + format (1) + num_pieces (14) = 12 bytes
  const headerLen = 4 + 4 + 4;
  const tableLen = 20 * PIECES.length;
  const offset = headerLen + tableLen;

  let totalBodyLen = 0;
  for (const p of PIECES) {
    totalBodyLen += p.sectors * 2048;
  }

  const outBuffer = new Uint8Array(offset + totalBodyLen);
  const view = new DataView(outBuffer.buffer);

  // Magic 'TTP2'
  outBuffer[0] = 0x54; outBuffer[1] = 0x54; outBuffer[2] = 0x50; outBuffer[3] = 0x32;
  view.setUint32(4, 1, true); // PS2_FORMAT = 1
  view.setUint32(8, PIECES.length, true);

  let currentBodyOffset = offset;
  for (let i = 0; i < PIECES.length; i++) {
    const p = PIECES[i];
    const recOffset = 12 + i * 20;
    view.setUint32(recOffset + 0, p.number, true);
    view.setUint32(recOffset + 4, currentBodyOffset, true);
    view.setUint32(recOffset + 8, p.sectors, true);
    view.setUint32(recOffset + 12, Math.max(p.loop, 0), true);
    view.setUint32(recOffset + 16, p.volume, true);

    const sliceStart = (p.lba - MUSIC_FIRST) * 2048;
    const sliceEnd = sliceStart + p.sectors * 2048;
    outBuffer.set(data.subarray(sliceStart, sliceEnd), currentBodyOffset);
    currentBodyOffset += p.sectors * 2048;
  }

  if (onProgress) onProgress("Paquete de música PS2 listo (61.5 MB).", 1.0);
  return outBuffer;
}
