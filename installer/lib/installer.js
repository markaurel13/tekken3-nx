// Core Installer Package Builder for Tekken 3 Expanded.
// Assembles all mod components, extracted PS2 audio, arcade stages and binaries into switch/tekken3/ on the SD card.

import { ZipWriter, zipSize } from './zip.js';
import { extractPs2Music } from './ps2.js';
import { t, formatSize } from './i18n.js';

export const SWITCH_ROOT = 'switch/tekken3';

async function fetchBytes(url) {
  const resp = await fetch(url);
  if (!resp.ok) {
    throw new Error(`Failed to load asset from ${url}: status ${resp.status}`);
  }
  return new Uint8Array(await resp.arrayBuffer());
}

async function fetchJson(url) {
  const resp = await fetch(url);
  if (!resp.ok) {
    throw new Error(`Failed to load manifest from ${url}: status ${resp.status}`);
  }
  return await resp.json();
}

export async function buildPackage(inputs, options = {}, sink, log = () => {}, progress = () => {}) {
  log("Starting Tekken 3 Expanded package generation...");
  progress(0.02);

  // 1. Load manifest
  log("Loading mod and binary manifest...");
  let manifest = null;
  try {
    manifest = await fetchJson('./release/manifest.json');
  } catch (err) {
    log(`Notice: local manifest not found, using default configuration.`);
  }

  const entriesToPack = [];

  // 2. PS2 Remixed Music (extracted directly from user's PS2 disc)
  if (inputs.ps2Disc) {
    log("Extracting remastered PS2 stage music directly from disc sectors...");
    const ps2MusicBytes = await extractPs2Music(inputs.ps2Disc, (msg, p) => {
      log(msg);
      progress(0.05 + p * 0.25);
    });
    entriesToPack.push({
      path: `${SWITCH_ROOT}/mods/ttt-ps2-music/ps2-music.ttp`,
      bytes: ps2MusicBytes
    });
    entriesToPack.push({
      path: `${SWITCH_ROOT}/mods/ttt-ps2-music/version.txt`,
      bytes: new TextEncoder().encode("1\n")
    });
    log(`Remastered PS2 audio successfully generated (${formatSize(ps2MusicBytes.length)}).`);
  }
  progress(0.35);

  // 4. Pre-built mod payloads from release/mods/ (All included for complete functionality)
  const modList = manifest?.mod_bundles || [
    { name: 'arcade-stages', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/arcade-stages.zip', target: 'mods/arcade-stages' },
    { name: 'ttt-stages', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/ttt-stages.zip', target: 'mods/ttt-stages' },
    { name: 'difficulty', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/difficulty.zip', target: 'mods/difficulty' },
    { name: 'ttt-music', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/ttt-music.zip', target: 'mods/ttt-music' },
    { name: 'ttt-ps2-laser', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/ttt-ps2-laser.zip', target: 'mods/ttt-ps2-laser' },
    { name: 'ttt-cinematics', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/ttt-cinematics.zip', target: 'mods/ttt-cinematics' },
    { name: 'ttt1-part1', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/ttt1-part1.zip', target: 'mods/ttt1' },
    { name: 'ttt1-part2', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/ttt1-part2.zip', target: 'mods/ttt1' },
    { name: 'ttt1-part3', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/ttt1-part3.zip', target: 'mods/ttt1' },
    { name: 'ttt1-part4', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/ttt1-part4.zip', target: 'mods/ttt1' },
    { name: 'ttt1-part5', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/ttt1-part5.zip', target: 'mods/ttt1' },
    { name: 'outfit-menu', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/outfit-menu.zip', target: 'mods/outfit-menu' },
    { name: 'anna-jessica-rabbit', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/anna-jessica-rabbit.zip', target: 'mods/anna-jessica-rabbit' },
    { name: 'eddy-monochrome', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/eddy-monochrome.zip', target: 'mods/eddy-monochrome' },
    { name: 'heihachi-tiger-coat', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/heihachi-tiger-coat.zip', target: 'mods/heihachi-tiger-coat' },
    { name: 'julia-blue', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/julia-blue.zip', target: 'mods/julia-blue' },
    { name: 'kuma-polar-bear', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/kuma-polar-bear.zip', target: 'mods/kuma-polar-bear' },
    { name: 'nina-white-satin', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/nina-white-satin.zip', target: 'mods/nina-white-satin' },
    { name: 'xiaoyu-cherry-blossom', path: 'https://archive.org/cors/tekken-3-definitive-edition-assets/xiaoyu-cherry-blossom.zip', target: 'mods/xiaoyu-cherry-blossom' }
  ];

  log("Downloading and integrating complete mod bundles...");
  let modProgress = 0.35;
  const modStep = 0.45 / Math.max(modList.length, 1);

  for (const mod of modList) {
    try {
      log(`Integrating mod: ${mod.name}...`);
      const modZipBytes = await fetchBytes(mod.path);
      const { readZipEntries } = await import('./zip_reader.js');
      const modBlob = new Blob([modZipBytes]);
      const modEntries = await readZipEntries(modBlob);
      for (const me of modEntries) {
        if (me.name.endsWith('/')) continue;
        const fileBytes = await me.extract();
        entriesToPack.push({
          path: `${SWITCH_ROOT}/${mod.target}/${me.name}`,
          bytes: fileBytes
        });
      }
      log(`Mod ${mod.name} integrated (${modEntries.length} files).`);
    } catch (err) {
      log(`Note: optional mod bundle (${mod.name}) not available: ${err.message}`);
    }
    modProgress += modStep;
    progress(modProgress);
  }

  // 5. Streaming write into final ZIP
  log("Calculating total package size...");
  const zipEntriesSummary = entriesToPack.map(e => ({
    path: e.path,
    size: e.bytes ? e.bytes.length : e.size
  }));

  const totalCalculated = zipSize(zipEntriesSummary);
  log(`Packing ${entriesToPack.length} files into Switch ZIP archive (${formatSize(totalCalculated)})...`);

  const zip = new ZipWriter(sink);
  let bytesDone = 0;

  for (let i = 0; i < entriesToPack.length; i++) {
    const entry = entriesToPack[i];
    if (entry.bytes) {
      await zip.addBytes(entry.path, entry.bytes);
      bytesDone += entry.bytes.length;
    } else if (entry.file) {
      await zip.add(entry.path, entry.file);
      bytesDone += entry.size;
    }
    progress(0.80 + (bytesDone / totalCalculated) * 0.19);
  }

  await zip.finish();
  progress(1.0);
  log("✓ Switch package created successfully! Download ready.");
}
