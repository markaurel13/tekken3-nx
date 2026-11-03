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

export async function buildPackage(inputs, options, sink, log = () => {}, progress = () => {}) {
  log("Iniciando generación de paquete Tekken 3 Expanded...");
  progress(0.02);

  // 1. Load manifest
  log("Cargando manifiesto de mods y binarios...");
  let manifest = null;
  try {
    manifest = await fetchJson('./release/manifest.json');
  } catch (err) {
    log(`Aviso: manifiesto local no encontrado, usando configuración por defecto.`);
  }

  const entriesToPack = [];

  // 2. Binary and config
  if (options.includeNro) {
    log("Añadiendo ejecutable de Nintendo Switch (tekken3_nx.nro)...");
    try {
      const nroBytes = await fetchBytes('./release/tekken3_nx.nro');
      entriesToPack.push({ path: `${SWITCH_ROOT}/tekken3_nx.nro`, bytes: nroBytes });
    } catch (e) {
      log("Aviso: tekken3_nx.nro no encontrado en release/, se omitirá en el ZIP.");
    }
  }

  // config.txt
  const configText = `# ==============================================================================
# Tekken 3 (USA) - Graphics Configuration for Nintendo Switch
# ==============================================================================
# Internal Supersampling Anti-Aliasing (SSAA):
#   1 = 1x PS1 Native (1024x512)
#   2 = 2x HD (2048x1024) [RECOMMENDED]
#   3 = 3x Ultra HD (3072x1536)
supersampling = 2

# Screen Aspect Ratio:
#   OFF = 4:3 Original with black sidebars [RECOMMENDED]
#   ON  = 16:9 Fullscreen Widescreen
widescreen_16_9 = OFF
`;
  entriesToPack.push({
    path: `${SWITCH_ROOT}/config.txt`,
    bytes: new TextEncoder().encode(configText)
  });

  // 3. User's PS1 Disc if provided
  if (inputs.ps1Files && inputs.ps1Files.length > 0) {
    for (const file of inputs.ps1Files) {
      log(`Añadiendo disco PS1: disc/${file.name} (${formatSize(file.size)})...`);
      entriesToPack.push({
        path: `${SWITCH_ROOT}/disc/${file.name}`,
        size: file.size,
        file: file,
      });
    }
  }

  // 4. PS2 Remixed Music (extracted directly from user's PS2 disc)
  if (options.includePs2Music && inputs.ps2Disc) {
    log("Extrayendo música de escenarios de PS2 remasterizada...");
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
    log(`Música PS2 compilada exitosamente (${formatSize(ps2MusicBytes.length)}).`);
  }
  progress(0.35);

  // 5. Pre-built mod payloads from release/mods/
  const modList = manifest?.mod_bundles || [
    { name: 'arcade-stages', path: './release/mods/arcade-stages.zip', target: 'mods/arcade-stages' },
    { name: 'ttt-stages', path: './release/mods/ttt-stages.zip', target: 'mods/ttt-stages' },
    { name: 'difficulty', path: './release/mods/difficulty.zip', target: 'mods/difficulty' },
    { name: 'ttt-music', path: './release/mods/ttt-music.zip', target: 'mods/ttt-music' },
    { name: 'ttt-ps2-laser', path: './release/mods/ttt-ps2-laser.zip', target: 'mods/ttt-ps2-laser' },
    { name: 'ttt-cinematics', path: './release/mods/ttt-cinematics.zip', target: 'mods/ttt-cinematics' },
    { name: 'ttt1', path: './release/mods/ttt1.zip', target: 'mods/ttt1' },
    { name: 'outfit-menu', path: './release/mods/outfit-menu.zip', target: 'mods/outfit-menu' }
  ];

  log("Descargando e integrando paquetes de mods...");
  let modProgress = 0.35;
  const modStep = 0.45 / Math.max(modList.length, 1);

  for (const mod of modList) {
    if (mod.name === 'arcade-stages' && !options.includeStages) continue;
    if (mod.name === 'ttt-stages' && !options.includeStages) continue;
    if (mod.name === 'ttt-cinematics' && !options.includeCinematics) continue;
    if (mod.name === 'outfit-menu' && !options.includeOutfits) continue;

    try {
      log(`Cargando mod: ${mod.name}...`);
      const modZipBytes = await fetchBytes(mod.path);
      // Unpack mod zip into entries
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
      log(`Mod ${mod.name} integrado (${modEntries.length} archivos).`);
    } catch (err) {
      log(`Nota: archivo opcional de mod (${mod.name}) no disponible: ${err.message}`);
    }
    modProgress += modStep;
    progress(modProgress);
  }

  // 6. Streaming write into final ZIP
  log("Calculando tamaño total del paquete ZIP...");
  const zipEntriesSummary = entriesToPack.map(e => ({
    path: e.path,
    size: e.bytes ? e.bytes.length : e.size
  }));

  const totalCalculated = zipSize(zipEntriesSummary);
  log(`Empaquetando ${entriesToPack.length} archivos en ZIP para Switch (${formatSize(totalCalculated)})...`);

  const zip = new ZipWriter(sink);
  let bytesDone = 0;

  for (const entry of entriesToPack) {
    if (entry.bytes) {
      await zip.addBytes(entry.path, entry.bytes);
      bytesDone += entry.bytes.length;
    } else if (entry.file) {
      // Chunked file stream
      const CHUNK = 8 * 1024 * 1024;
      const file = entry.file;
      async function* chunks() {
        for (let off = 0; off < file.size; off += CHUNK) {
          const slice = file.slice(off, Math.min(file.size, off + CHUNK));
          yield new Uint8Array(await slice.arrayBuffer());
        }
      }
      await zip.addFile(entry.path, file.size, chunks());
      bytesDone += file.size;
    }
    progress(0.80 + (bytesDone / Math.max(totalCalculated, 1)) * 0.19);
  }

  log("Finalizando cabeceras y tabla central de ZIP...");
  await zip.finish();
  progress(1.0);
  log("¡Paquete tekken3-expanded.zip listo para descarga!");
}
