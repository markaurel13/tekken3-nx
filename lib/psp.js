// PSP UMD Disc Verifier for Tekken 5: Dark Resurrection (PSP).
// Reads ISO-9660 UMD_DATA.BIN to verify the official Tekken 5 DR product code.

import { DiscReader } from './iso.js';

const T5DR_CODES = {
  'ULUS-10139': 'Tekken: Dark Resurrection (USA) [ULUS-10139]',
  'ULES-00356': 'Tekken: Dark Resurrection (EUR) [ULES-00356]',
  'ULJS-00073': 'Tekken: Dark Resurrection (JPN) [ULJS-00073]',
  'ULKS-46082': 'Tekken: Dark Resurrection (KOR) [ULKS-46082]',
  'ULAS-42058': 'Tekken: Dark Resurrection (ASIA) [ULAS-42058]',
};

export async function identifyT5DrPspDisc(blob) {
  const reader = new DiscReader(blob);
  await reader.init();

  const files = await reader.listFiles();
  const hasPspGame = files.some(f => f.name.toUpperCase() === 'PSP_GAME' && f.isDirectory);
  const umdData = files.find(f => f.name.toUpperCase() === 'UMD_DATA.BIN' && !f.isDirectory);

  if (!hasPspGame || !umdData) {
    return { valid: false, reason: 'Not a PSP UMD ISO image' };
  }

  const sector = await reader.readSectors(umdData.lba, 1);
  const text = String.fromCharCode(...sector.subarray(0, 64)).toUpperCase();

  for (const [code, edition] of Object.entries(T5DR_CODES)) {
    if (text.includes(code)) {
      return { valid: true, edition, code };
    }
  }

  return { valid: false, reason: 'PSP UMD detected, but not Tekken: Dark Resurrection' };
}
