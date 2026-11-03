// Web Worker for off-thread file analysis, ROM verification, and package creation.

import { identifyArcadeZip } from './lib/arcade.js';
import { identifyTttPs2Disc } from './lib/ps2.js';
import { buildPackage } from './lib/installer.js';
import { readZipEntries } from './lib/zip_reader.js';

const log = (text) => postMessage({ type: 'log', text });
const progress = (fraction) => postMessage({ type: 'progress', fraction });

let inspectedFiles = {
  t3Arcade: null,
  tttArcade: null,
  ps2Disc: null,
  ps1Files: [],
};

self.addEventListener('message', async (event) => {
  const data = event.data || {};

  if (data.type === 'inspect') {
    const fileList = data.files || [];
    log(`Examinando ${fileList.length} archivo(s) arrastrado(s)...`);
    inspectedFiles = {
      t3Arcade: null,
      tttArcade: null,
      ps2Disc: null,
      ps1Files: [],
    };

    for (const file of fileList) {
      const name = file.name.toLowerCase();
      log(`Analizando: ${file.name} (${(file.size / 1024 / 1024).toFixed(1)} MB)...`);

      // 1. Check for ZIP
      if (name.endsWith('.zip')) {
        const arc = await identifyArcadeZip(file);
        if (arc.type === 'tekken3') {
          inspectedFiles.t3Arcade = file;
          log(`✓ Tekken 3 Arcade ROM detectado (${file.name})`);
          continue;
        } else if (arc.type === 'tektagt') {
          inspectedFiles.tttArcade = file;
          log(`✓ Tekken Tag Tournament Arcade ROM detectado (${file.name})`);
          continue;
        }

        // Could be an all-in-one ZIP archive
        try {
          const innerEntries = await readZipEntries(file);
          log(`Inspeccionando contenido de ${file.name} (${innerEntries.length} entradas)...`);
          for (const entry of innerEntries) {
            const ename = entry.name.toLowerCase();
            if (ename.includes('tekken3') && ename.endsWith('.zip')) {
              const innerBytes = await entry.extract();
              const innerBlob = new Blob([innerBytes]);
              inspectedFiles.t3Arcade = innerBlob;
              log(`✓ Tekken 3 Arcade ROM extraído de ${entry.name}`);
            } else if (ename.includes('tektagt') && ename.endsWith('.zip')) {
              const innerBytes = await entry.extract();
              const innerBlob = new Blob([innerBytes]);
              inspectedFiles.tttArcade = innerBlob;
              log(`✓ Tekken Tag Arcade ROM extraído de ${entry.name}`);
            }
          }
        } catch (e) {
          // not a nested zip or cannot parse
        }
      }

      // 2. Check for PS2 ISO / BIN / CUE
      if (name.endsWith('.iso') || name.endsWith('.bin') || name.endsWith('.cue') || name.endsWith('.img')) {
        // Test if PS2
        try {
          const ps2Check = await identifyTttPs2Disc(file);
          if (ps2Check.valid) {
            inspectedFiles.ps2Disc = file;
            log(`✓ Disco PS2 Tekken Tag Tournament (USA) detectado (${file.name})`);
            continue;
          }
        } catch (e) {
          // not PS2
        }

        // Test if PS1
        if (name.includes('tekken 3') || name.includes('slus-00402') || name.includes('slus_004.02') || name.includes('tekken3')) {
          inspectedFiles.ps1Files.push(file);
          log(`✓ Disco PS1 Tekken 3 detectado (${file.name})`);
          continue;
        }
      }
    }

    postMessage({
      type: 'inspected',
      hasT3Arcade: inspectedFiles.t3Arcade !== null,
      hasTttArcade: inspectedFiles.tttArcade !== null,
      hasPs2Disc: inspectedFiles.ps2Disc !== null,
      hasPs1Disc: inspectedFiles.ps1Files.length > 0,
      t3ArcadeName: inspectedFiles.t3Arcade?.name || 'tekken3.zip',
      tttArcadeName: inspectedFiles.tttArcade?.name || 'tektagt.zip',
      ps2DiscName: inspectedFiles.ps2Disc?.name || 'Tekken Tag (USA).iso',
      ps1FilesCount: inspectedFiles.ps1Files.length,
    });
  }

  else if (data.type === 'start-build') {
    const options = data.options || {};
    const downloadPort = event.ports && event.ports[0];

    // Sink interface for ZipWriter
    let sink;
    if (downloadPort) {
      // Streamed through Service Worker MessagePort
      let pendingPull = null;
      downloadPort.onmessage = ({ data: msg }) => {
        if (msg.type === 'pull' && pendingPull) {
          const resolve = pendingPull;
          pendingPull = null;
          resolve();
        }
      };

      sink = {
        async write(bytes) {
          await new Promise((resolve) => {
            pendingPull = resolve;
            downloadPort.postMessage({ type: 'chunk', buffer: bytes.buffer }, [bytes.buffer]);
          });
        },
        async close() {
          downloadPort.postMessage({ type: 'end' });
        }
      };
    } else {
      // Memory chunk buffer fallback
      const chunks = [];
      sink = {
        async write(bytes) {
          chunks.push(bytes.slice());
        },
        async close() {
          const blob = new Blob(chunks, { type: 'application/zip' });
          postMessage({ type: 'fallback-download', blob });
        }
      };
    }

    try {
      await buildPackage(inspectedFiles, options, sink, log, progress);
      postMessage({ type: 'build-finished' });
    } catch (err) {
      log(`ERROR: ${err.message}`);
      postMessage({ type: 'build-error', message: err.message });
      if (downloadPort) {
        downloadPort.postMessage({ type: 'error', message: err.message });
      }
    }
  }
});
