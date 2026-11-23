// Web Worker for off-thread file analysis, ROM verification, and package creation.

import { identifyArcadeZip } from './lib/arcade.js?v=1.5.1';
import { identifyTttPs2Disc } from './lib/ps2.js?v=1.5.1';
import { identifyT5DrPspDisc } from './lib/psp.js?v=1.5.1';
import { buildPackage } from './lib/installer.js?v=1.5.1';
import { readZipEntries } from './lib/zip_reader.js?v=1.5.1';

const log = (text) => postMessage({ type: 'log', text });
const progress = (fraction) => postMessage({ type: 'progress', fraction });

let inspectedFiles = {
  t3Arcade: null,
  tttArcade: null,
  ps2Disc: null,
  t5DrDisc: null,
};

self.addEventListener('message', async (event) => {
  const data = event.data || {};

  if (data.type === 'clear') {
    inspectedFiles = {
      t3Arcade: null,
      tttArcade: null,
      ps2Disc: null,
      t5DrDisc: null,
    };
    log("Files cleared. Ready to accept new files.");
    postMessage({
      type: 'inspected',
      hasT3Arcade: false,
      hasTttArcade: false,
      hasPs2Disc: false,
      hasT5DrDisc: false,
      t3ArcadeName: '',
      tttArcadeName: '',
      ps2DiscName: '',
      t5DrDiscName: '',
    });
    return;
  }

  if (data.type === 'inspect') {
    const fileList = data.files || [];
    log(`Inspecting ${fileList.length} file(s)...`);

    for (const file of fileList) {
      const name = file.name.toLowerCase();
      log(`Analyzing: ${file.name} (${(file.size / 1024 / 1024).toFixed(1)} MB)...`);

      // 1. Check for ZIP
      if (name.endsWith('.zip')) {
        const arc = await identifyArcadeZip(file);
        if (arc.type === 'tekken3') {
          inspectedFiles.t3Arcade = file;
          log(`✓ Tekken 3 Arcade ROM detected (${file.name})`);
          continue;
        } else if (arc.type === 'tektagt') {
          inspectedFiles.tttArcade = file;
          log(`✓ Tekken Tag Tournament Arcade ROM detected (${file.name})`);
          continue;
        }

        // Could be an all-in-one ZIP archive
        try {
          const innerEntries = await readZipEntries(file);
          log(`Inspecting contents of archive ${file.name} (${innerEntries.length} entries)...`);
          for (const entry of innerEntries) {
            const ename = entry.name.toLowerCase();
            if (ename.includes('tekken3') && ename.endsWith('.zip')) {
              const innerBytes = await entry.extract();
              const innerBlob = new Blob([innerBytes]);
              inspectedFiles.t3Arcade = innerBlob;
              log(`✓ Tekken 3 Arcade ROM extracted from ${entry.name}`);
            } else if (ename.includes('tektagt') && ename.endsWith('.zip')) {
              const innerBytes = await entry.extract();
              const innerBlob = new Blob([innerBytes]);
              inspectedFiles.tttArcade = innerBlob;
              log(`✓ Tekken Tag Arcade ROM extracted from ${entry.name}`);
            }
          }
        } catch (e) {
          // not a nested zip or cannot parse
        }
      }

      // 2. Check for PS2 ISO / BIN / CUE
      if (name.endsWith('.iso') || name.endsWith('.bin') || name.endsWith('.cue') || name.endsWith('.img')) {
        try {
          const ps2Check = await identifyTttPs2Disc(file);
          if (ps2Check.valid) {
            inspectedFiles.ps2Disc = file;
            log(`✓ Tekken Tag Tournament (USA) PS2 Disc image detected (${file.name})`);
            continue;
          } else if (ps2Check.reason && ps2Check.reason.includes("Incompatible region")) {
            log(`✗ Incompatible PS2 Disc: Only the USA version (SLUS-20001) is supported. European or Japanese discs will not work.`);
          }
        } catch (e) {
          // not PS2
        }
        try {
          const pspCheck = await identifyT5DrPspDisc(file);
          if (pspCheck.valid) {
            inspectedFiles.t5DrDisc = file;
            log(`✓ ${pspCheck.edition} PSP ISO detected (${file.name})`);
            continue;
          }
        } catch (e) {
          // not PSP
        }
      }
    }

    postMessage({
      type: 'inspected',
      hasT3Arcade: inspectedFiles.t3Arcade !== null,
      hasTttArcade: inspectedFiles.tttArcade !== null,
      hasPs2Disc: inspectedFiles.ps2Disc !== null,
      hasT5DrDisc: inspectedFiles.t5DrDisc !== null,
      t3ArcadeName: inspectedFiles.t3Arcade?.name || 'tekken3.zip',
      tttArcadeName: inspectedFiles.tttArcade?.name || 'tektagt.zip',
      ps2DiscName: inspectedFiles.ps2Disc?.name || 'Tekken Tag (USA).iso',
      t5DrDiscName: inspectedFiles.t5DrDisc?.name || 'Tekken 5 DR (PSP).iso',
    });
  }

  else if (data.type === 'start-build') {
    const options = data.options || {};
    const streamChunks = Boolean(options.streamChunks);
    const BATCH_BYTES = 4 * 1024 * 1024;
    let batchParts = [];
    let batchLen = 0;

    function flushBatch() {
      if (batchLen === 0) return;
      const merged = new Uint8Array(batchLen);
      let offset = 0;
      for (const part of batchParts) {
        merged.set(part, offset);
        offset += part.length;
      }
      batchParts = [];
      batchLen = 0;
      postMessage({ type: 'chunk', buffer: merged.buffer }, [merged.buffer]);
    }

    const chunks = streamChunks ? null : [];
    const sink = {
      async write(bytes) {
        const copy = bytes.slice();
        if (streamChunks) {
          batchParts.push(copy);
          batchLen += copy.length;
          if (batchLen >= BATCH_BYTES) {
            flushBatch();
          }
        } else {
          chunks.push(copy);
        }
      },
      async close() {
        if (streamChunks) {
          flushBatch();
          postMessage({ type: 'build-finished', streamed: true });
        } else {
          const blob = new Blob(chunks, { type: 'application/zip' });
          postMessage({ type: 'build-finished', blob });
        }
      }
    };

    try {
      await buildPackage(inspectedFiles, options, sink, log, progress);
    } catch (err) {
      log(`Error while building package: ${err.message}`);
      postMessage({ type: 'build-error', message: err.message });
    }
  }
});
