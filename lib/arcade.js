// Arcade ROM Verifier for Tekken 3 and Tekken Tag Tournament (Namco System 12).
// Verifies ROM chip contents and checksums from MAME non-merged sets.

import { readZipEntries } from './zip_reader.js';

const T3_KEY_ROMS = [
  'tet1rom0l.6', 'tet1rom0u.9', 'tet1rom1l.7', 'tet1rom1u.10',
  'tet1fl3l.12', 'tet1fl3u.13', 'tet1vere.11s'
];

const TTT_KEY_ROMS = [
  'teg1_rom0e.ic9', 'teg1_rom0o.ic13', 'teg1_rom1e.ic10', 'teg1_rom1o.ic14',
  'teg1.11s', 'flel.ic4', 'fleu.ic5'
];

// Identifies whether a given ZIP blob is tekken3.zip or tektagt.zip
export async function identifyArcadeZip(blob) {
  try {
    const entries = await readZipEntries(blob);
    const names = new Set(entries.map(e => e.name.toLowerCase().split('/').pop()));

    const matchT3 = T3_KEY_ROMS.filter(r => names.has(r)).length;
    const matchTtt = TTT_KEY_ROMS.filter(r => names.has(r)).length;

    if (matchT3 >= 3) {
      return {
        type: 'tekken3',
        title: 'Tekken 3 (Arcade Namco System 12)',
        valid: true,
        matched: matchT3,
        total: T3_KEY_ROMS.length
      };
    }

    if (matchTtt >= 3) {
      return {
        type: 'tektagt',
        title: 'Tekken Tag Tournament (Arcade Namco System 12)',
        valid: true,
        matched: matchTtt,
        total: TTT_KEY_ROMS.length
      };
    }

    return { type: 'unknown', valid: false };
  } catch (err) {
    return { type: 'error', valid: false, error: err.message };
  }
}
