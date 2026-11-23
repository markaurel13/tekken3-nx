// Core Installer Package Builder for Tekken 3 Expanded.
// Assembles all mod components, extracted PS2 audio, arcade stages and binaries into switch/tekken3/ on the SD card.

import { ZipWriter } from './zip.js?v=1.5.2';
import { extractPs2Music } from './ps2.js?v=1.5.2';
import { t, formatSize } from './i18n.js?v=1.5.2';

export const SWITCH_ROOT = 'switch/tekken3';
const CACHE_BUST = 'v=1.5.2';

function withBust(url) {
  return url.includes('?') ? `${url}&${CACHE_BUST}` : `${url}?${CACHE_BUST}`;
}

async function fetchBytes(url) {
  const resp = await fetch(withBust(url), { cache: 'no-cache' });
  if (!resp.ok) {
    throw new Error(`Failed to load asset from ${url}: status ${resp.status}`);
  }
  return new Uint8Array(await resp.arrayBuffer());
}

async function fetchJson(url) {
  const resp = await fetch(withBust(url), { cache: 'no-cache' });
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

  const zip = new ZipWriter(sink);
  let totalFilesPacked = 0;
  let totalBytesPacked = 0;

  // 2. PS2 Remixed Music (extracted directly from user's PS2 disc and streamed straight into ZIP)
  if (inputs.ps2Disc) {
    log("Extracting remastered PS2 stage music directly from disc sectors...");
    const ps2MusicBytes = await extractPs2Music(inputs.ps2Disc, (msg, p) => {
      log(msg);
      progress(0.05 + p * 0.25);
    });
    const verBytes = new TextEncoder().encode("1\n");
    await zip.addBytes(`${SWITCH_ROOT}/mods/ttt-ps2-music/ps2-music.ttp`, ps2MusicBytes);
    await zip.addBytes(`${SWITCH_ROOT}/mods/ttt-ps2-music/version.txt`, verBytes);
    totalFilesPacked += 2;
    totalBytesPacked += ps2MusicBytes.length + verBytes.length;
    log(`Remastered PS2 audio successfully generated (${formatSize(ps2MusicBytes.length)}).`);
  }
  progress(0.32);

  // 4. Pre-built mod payloads from release/mods/ and release/addons/ (streamed one by one for minimal RAM)
  const modList = manifest?.mod_bundles || [
    { name: 'arcade-stages', path: './release/mods/arcade-stages.zip', target: 'mods/arcade-stages' },
    { name: 'ttt-stages', path: './release/mods/ttt-stages.zip', target: 'mods/ttt-stages' },
    { name: 'difficulty', path: './release/mods/difficulty.zip', target: 'mods/difficulty' },
    { name: 'ttt-music', path: './release/mods/ttt-music.zip', target: 'mods/ttt-music' },
    { name: 'ttt-ps2-laser', path: './release/mods/ttt-ps2-laser.zip', target: 'mods/ttt-ps2-laser' },
    { name: 'ttt-cinematics', path: './release/mods/ttt-cinematics.zip', target: 'mods/ttt-cinematics' },
    { name: 'ttt1-part1', path: './release/mods/ttt1-part1.zip', target: 'mods/ttt1' },
    { name: 'ttt1-part2', path: './release/mods/ttt1-part2.zip', target: 'mods/ttt1' },
    { name: 'ttt1-part3', path: './release/mods/ttt1-part3.zip', target: 'mods/ttt1' },
    { name: 'ttt1-part4', path: './release/mods/ttt1-part4.zip', target: 'mods/ttt1' },
    { name: 'ttt1-part5', path: './release/mods/ttt1-part5.zip', target: 'mods/ttt1' },
    { name: 'ttt1-part6', path: './release/mods/ttt1-part6.zip', target: 'mods/ttt1' },
    { name: 't2-stages', path: './release/mods/t2-stages.zip', target: 'mods/t2-stages' },
    { name: 'tmst-data', path: './release/mods/tmst-data.zip', target: 'mods/tmst-data' },
    { name: 'outfit-menu', path: './release/mods/outfit-menu.zip', target: 'mods/outfit-menu' },
    { name: 'anna-jessica-rabbit', path: './release/mods/anna-jessica-rabbit.zip', target: 'mods/anna-jessica-rabbit' },
    { name: 'eddy-monochrome', path: './release/mods/eddy-monochrome.zip', target: 'mods/eddy-monochrome' },
    { name: 'heihachi-tiger-coat', path: './release/mods/heihachi-tiger-coat.zip', target: 'mods/heihachi-tiger-coat' },
    { name: 'julia-blue', path: './release/mods/julia-blue.zip', target: 'mods/julia-blue' },
    { name: 'kuma-polar-bear', path: './release/mods/kuma-polar-bear.zip', target: 'mods/kuma-polar-bear' },
    { name: 'nina-white-satin', path: './release/mods/nina-white-satin.zip', target: 'mods/nina-white-satin' },
    { name: 'xiaoyu-cherry-blossom', path: './release/mods/xiaoyu-cherry-blossom.zip', target: 'mods/xiaoyu-cherry-blossom' },
    { name: 'preloaded', path: './release/mods/preloaded.zip', target: 'mods/preloaded' },
    { name: 'local-packages', path: './release/mods/local-packages.zip', target: 'mods/local-packages' },
    { name: 'examples', path: './release/mods/examples.zip', target: 'mods/examples' },
    { name: 'test-packages', path: './release/mods/test-packages.zip', target: 'mods/test-packages' },
    { name: 'tmst-workspace-part1', path: './release/addons/tmst-workspace-part1.zip', target: 'addons/tmst/workspace' },
    { name: 'tmst-workspace-part2', path: './release/addons/tmst-workspace-part2.zip', target: 'addons/tmst/workspace' },
    { name: 'tmst-workspace-part3', path: './release/addons/tmst-workspace-part3.zip', target: 'addons/tmst/workspace' },
    { name: 'tmst-workspace-part4', path: './release/addons/tmst-workspace-part4.zip', target: 'addons/tmst/workspace' },
    { name: 'tmst-workspace-part5', path: './release/addons/tmst-workspace-part5.zip', target: 'addons/tmst/workspace' },
    { name: 'tmst-workspace-part6', path: './release/addons/tmst-workspace-part6.zip', target: 'addons/tmst/workspace' },
    { name: 'tmst-workspace-part7', path: './release/addons/tmst-workspace-part7.zip', target: 'addons/tmst/workspace' },
    { name: 'tmst-workspace-part8', path: './release/addons/tmst-workspace-part8.zip', target: 'addons/tmst/workspace' },
    { name: 'tmst-workspace-part9', path: './release/addons/tmst-workspace-part9.zip', target: 'addons/tmst/workspace' },
    { name: 'tmst-workspace-part10', path: './release/addons/tmst-workspace-part10.zip', target: 'addons/tmst/workspace' },
    { name: 'tmst-workspace-part11', path: './release/addons/tmst-workspace-part11.zip', target: 'addons/tmst/workspace' },
    { name: 'tmst-workspace-part12', path: './release/addons/tmst-workspace-part12.zip', target: 'addons/tmst/workspace' }
  ];

  log("Downloading and streaming mod bundles directly into Switch ZIP...");
  const { readZipEntries } = await import('./zip_reader.js?v=1.5.2');
  let modProgress = 0.32;
  const modStep = 0.58 / Math.max(modList.length, 1);

  for (const mod of modList) {
    try {
      log(`Integrating mod: ${mod.name}...`);
      const modZipBytes = await fetchBytes(mod.path);
      const modBlob = new Blob([modZipBytes]);
      const modEntries = await readZipEntries(modBlob);
      let count = 0;
      for (const me of modEntries) {
        if (me.name.endsWith('/')) continue;
        const fileBytes = await me.extract();
        await zip.addBytes(`${SWITCH_ROOT}/${mod.target}/${me.name}`, fileBytes);
        totalFilesPacked++;
        totalBytesPacked += fileBytes.length;
        count++;
      }
      log(`Mod ${mod.name} packed (${count} files).`);
    } catch (err) {
      log(`Note: optional mod bundle (${mod.name}) not available: ${err.message}`);
    }
    modProgress += modStep;
    progress(modProgress);
  }

  log(`Packed ${totalFilesPacked} files (${formatSize(totalBytesPacked)}) into Switch ZIP archive.`);
  await zip.finish();
  progress(0.90);
}
