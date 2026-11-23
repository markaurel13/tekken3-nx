// Main User Interface Controller for Tekken 3 Expanded Web Installer.
// Handles Drag & Drop, file inspections, visual status badges, and Service Worker download coordination.

import { t, formatSize } from './lib/i18n.js';

const dropZone = document.getElementById('dropZone');
const fileInput = document.getElementById('fileInput');
const folderInput = document.getElementById('folderInput');
const btnClear = document.getElementById('btnClear');
const btnBuild = document.getElementById('btnBuild');
const btnDownloadAgain = document.getElementById('btnDownloadAgain');
const progressContainer = document.getElementById('progressContainer');
const progressBar = document.getElementById('progressBar');
const logBox = document.getElementById('logBox');
const resultCard = document.getElementById('resultCard');

// Badges
const badgeT3Arcade = document.getElementById('badgeT3Arcade');
const badgeTttArcade = document.getElementById('badgeTttArcade');
const badgeTttPs2 = document.getElementById('badgeTttPs2');
const badgeT5Dr = document.getElementById('badgeT5Dr');

let worker = null;
let inspectedState = null;
let lastBuiltBlob = null;
let lastBlobUrl = null;
let activeWritable = null;
let activeSwPort = null;
let savedFileName = '';
let writeChain = Promise.resolve();
let chunksQueued = 0;
let chunksWritten = 0;
let isFinalizingDisk = false;

function triggerDownload(blob, filename = 'tekken3-expanded.zip') {
  if (lastBlobUrl) {
    try { URL.revokeObjectURL(lastBlobUrl); } catch (e) {}
  }
  const url = URL.createObjectURL(blob);
  lastBlobUrl = url;
  const a = document.createElement('a');
  a.href = url;
  a.download = filename;
  document.body.appendChild(a);
  a.click();
  try { document.body.removeChild(a); } catch (e) {}
}

async function startSwStreamDownload(filename = 'tekken3-expanded.zip') {
  const sw = navigator.serviceWorker?.controller;
  if (!sw) return null;
  const id = `${Date.now()}-${Math.random().toString(36).slice(2, 10)}`;
  const dataChannel = new MessageChannel();
  const ackChannel = new MessageChannel();
  await new Promise((resolve, reject) => {
    const timer = setTimeout(() => reject(new Error('SW ack timeout')), 3000);
    ackChannel.port2.onmessage = () => {
      clearTimeout(timer);
      resolve();
    };
    sw.postMessage({ type: 'download', id, filename }, [dataChannel.port1, ackChannel.port1]);
  });
  const a = document.createElement('a');
  a.href = `./download/${id}/${filename}`;
  a.download = filename;
  document.body.appendChild(a);
  a.click();
  try { document.body.removeChild(a); } catch (e) {}
  return dataChannel.port2;
}

function appendLog(text, isError = false) {
  const line = document.createElement('div');
  line.className = isError ? 'log-line error' : 'log-line';
  line.textContent = `[${new Date().toLocaleTimeString()}] ${text}`;
  logBox.appendChild(line);
  logBox.scrollTop = logBox.scrollHeight;
}

function updateBadge(badgeEl, state, detail = '') {
  badgeEl.className = `status-badge ${state}`;
  if (state === 'good') {
    badgeEl.textContent = `✓ Detected ${detail ? `(${detail})` : ''}`;
  } else if (state === 'checking') {
    badgeEl.textContent = `⏳ Checking...`;
  } else {
    badgeEl.textContent = `✗ Not detected`;
  }
}

function checkBuildReadiness() {
  if (!inspectedState) {
    btnBuild.disabled = true;
    return;
  }
  // All 4 files are strictly required to build the full expansion package
  const canBuild = inspectedState.hasT3Arcade && inspectedState.hasTttArcade &&
                   inspectedState.hasPs2Disc && inspectedState.hasT5DrDisc;
  btnBuild.disabled = !canBuild;
}

// Worker initialization
function initWorker() {
  if (worker) return;
  worker = new Worker('./worker.js', { type: 'module' });

  worker.addEventListener('message', async (event) => {
    const data = event.data || {};
    if (data.type === 'log') {
      appendLog(data.text);
    } else if (data.type === 'progress') {
      const pct = Math.round(data.fraction * 100);
      progressBar.style.width = `${pct}%`;
      progressBar.textContent = `${pct}%`;
    } else if (data.type === 'inspected') {
      inspectedState = data;
      updateBadge(badgeT3Arcade, data.hasT3Arcade ? 'good' : 'bad', data.t3ArcadeName);
      updateBadge(badgeTttArcade, data.hasTttArcade ? 'good' : 'bad', data.tttArcadeName);
      updateBadge(badgeTttPs2, data.hasPs2Disc ? 'good' : 'bad', data.ps2DiscName);
      updateBadge(badgeT5Dr, data.hasT5DrDisc ? 'good' : 'bad', data.t5DrDiscName);
      checkBuildReadiness();
      if (btnClear) {
        btnClear.style.display = (data.hasT3Arcade || data.hasTttArcade || data.hasPs2Disc || data.hasT5DrDisc) ? 'inline-flex' : 'none';
      }
    } else if (data.type === 'chunk') {
      if (activeWritable) {
        const buf = data.buffer;
        chunksQueued++;
        writeChain = writeChain.then(async () => {
          await activeWritable.write(new Uint8Array(buf));
          chunksWritten++;
          if (isFinalizingDisk && chunksQueued > 0) {
            const pct = 90 + Math.round((chunksWritten / chunksQueued) * 9);
            progressBar.style.width = `${pct}%`;
            progressBar.textContent = `${pct}%`;
          }
        });
      } else if (activeSwPort) {
        activeSwPort.postMessage({ type: 'chunk', buffer: data.buffer }, [data.buffer]);
      }
    } else if (data.type === 'build-finished') {
      try {
        const wasSavedDirectly = Boolean(activeWritable);
        if (activeWritable) {
          isFinalizingDisk = true;
          appendLog(`Finalizing ZIP file on disk (${chunksWritten}/${chunksQueued} blocks)...`);
          await writeChain;
          progressBar.style.width = '99%';
          progressBar.textContent = '99%';
          await activeWritable.close();
          activeWritable = null;
          isFinalizingDisk = false;
        }
        if (activeSwPort) {
          activeSwPort.postMessage({ type: 'end' });
          activeSwPort = null;
        }
        progressBar.style.width = '100%';
        progressBar.textContent = '100%';
        resultCard.hidden = false;
        const resultSummary = document.getElementById('resultSummary');
        if (wasSavedDirectly) {
          const fname = savedFileName || 'tekken3-expanded.zip';
          appendLog(`✓ Build complete! Check "${fname}" in the folder where you saved it at the start.`);
          if (resultSummary) {
            resultSummary.textContent = '';
            const strong = document.createElement('strong');
            strong.textContent = fname;
            resultSummary.append('Your file ', strong, ' has been saved directly to the folder you selected at the start. Check that folder, extract it, and copy the ');
            const code1 = document.createElement('code');
            code1.textContent = 'switch';
            const code2 = document.createElement('code');
            code2.textContent = 'sdmc:/';
            resultSummary.append(code1, ' folder directly to the root of your microSD card (', code2, ').');
          }
        } else {
          appendLog("✓ Build complete! Check your browser Downloads folder for tekken3-expanded.zip.");
        }
        if (data.blob) {
          lastBuiltBlob = data.blob;
          if (btnDownloadAgain) btnDownloadAgain.hidden = false;
          triggerDownload(data.blob);
        } else {
          lastBuiltBlob = null;
          if (btnDownloadAgain) btnDownloadAgain.hidden = true;
        }
      } catch (err) {
        appendLog(`Error saving file: ${err.message}`, true);
        if (activeWritable) {
          try { await activeWritable.abort(); } catch (e) {}
          activeWritable = null;
        }
      } finally {
        btnBuild.disabled = false;
        btnBuild.textContent = "⚡ Build Switch Package";
      }
    } else if (data.type === 'build-error') {
      if (activeWritable) {
        try { await activeWritable.abort(); } catch (e) {}
        activeWritable = null;
      }
      if (activeSwPort) {
        activeSwPort.postMessage({ type: 'error', message: data.message });
        activeSwPort = null;
      }
      btnBuild.disabled = false;
      btnBuild.textContent = "⚡ Build Switch Package";
      appendLog(data.message, true);
    }
  });
}
initWorker();

// Optional background service worker for offline cache
if ('serviceWorker' in navigator) {
  navigator.serviceWorker.register('./sw.js').catch(() => {});
}

// Drag & Drop handlers
async function handleDroppedItems(items) {
  const files = [];

  async function traverse(item) {
    if (item.isFile) {
      const file = await new Promise((resolve) => item.file(resolve));
      files.push(file);
    } else if (item.isDirectory) {
      const reader = item.createReader();
      const entries = await new Promise((resolve) => reader.readEntries(resolve));
      for (const entry of entries) {
        await traverse(entry);
      }
    }
  }

  for (const it of items) {
    if (typeof it.webkitGetAsEntry === 'function') {
      const entry = it.webkitGetAsEntry();
      if (entry) await traverse(entry);
      else if (it.getAsFile) files.push(it.getAsFile());
    } else if (it instanceof File) {
      files.push(it);
    }
  }

  if (files.length > 0) {
    resultCard.hidden = true;
    progressContainer.hidden = true;
    progressBar.style.width = '0%';
    progressBar.textContent = '0%';
    logBox.innerHTML = '';

    if (!inspectedState?.hasT3Arcade) updateBadge(badgeT3Arcade, 'checking');
    if (!inspectedState?.hasTttArcade) updateBadge(badgeTttArcade, 'checking');
    if (!inspectedState?.hasPs2Disc) updateBadge(badgeTttPs2, 'checking');
    if (!inspectedState?.hasT5DrDisc) updateBadge(badgeT5Dr, 'checking');
    worker.postMessage({ type: 'inspect', files });
  }
}

['dragenter', 'dragover'].forEach(evt => {
  window.addEventListener(evt, (e) => {
    e.preventDefault();
    dropZone.classList.add('drag-active');
  });
});

['dragleave', 'drop'].forEach(evt => {
  window.addEventListener(evt, (e) => {
    e.preventDefault();
    dropZone.classList.remove('drag-active');
  });
});

dropZone.addEventListener('drop', async (e) => {
  e.preventDefault();
  dropZone.classList.remove('drag-active');
  const items = e.dataTransfer.items ? [...e.dataTransfer.items] : [...e.dataTransfer.files];
  await handleDroppedItems(items);
});

fileInput.addEventListener('change', (e) => {
  if (e.target.files.length) handleDroppedItems([...e.target.files]);
  fileInput.value = '';
});

folderInput.addEventListener('change', (e) => {
  if (e.target.files.length) handleDroppedItems([...e.target.files]);
  folderInput.value = '';
});

if (btnClear) {
  btnClear.addEventListener('click', () => {
    resultCard.hidden = true;
    progressContainer.hidden = true;
    progressBar.style.width = '0%';
    progressBar.textContent = '0%';
    logBox.innerHTML = '';
    btnBuild.disabled = true;
    worker.postMessage({ type: 'clear' });
  });
}

// Build button click
btnBuild.addEventListener('click', async () => {
  activeWritable = null;
  activeSwPort = null;
  savedFileName = '';
  chunksQueued = 0;
  chunksWritten = 0;
  isFinalizingDisk = false;
  writeChain = Promise.resolve();

  if (typeof window.showSaveFilePicker === 'function') {
    try {
      const handle = await window.showSaveFilePicker({
        suggestedName: 'tekken3-expanded.zip',
        types: [{ description: 'ZIP Archive', accept: { 'application/zip': ['.zip'] } }],
      });
      savedFileName = handle.name || 'tekken3-expanded.zip';
      activeWritable = await handle.createWritable();
    } catch (err) {
      if (err && err.name === 'AbortError') return;
      activeWritable = null;
      savedFileName = '';
    }
  }

  if (!activeWritable) {
    try {
      activeSwPort = await startSwStreamDownload('tekken3-expanded.zip');
    } catch (err) {
      activeSwPort = null;
    }
  }

  btnBuild.disabled = true;
  btnBuild.textContent = "⏳ Building Package...";
  progressContainer.hidden = false;
  progressBar.style.width = '0%';
  progressBar.textContent = '0%';
  resultCard.hidden = true;
  logBox.innerHTML = '';

  // All expansion components are included
  const options = {
    includePs2Music: true,
    includeStages: true,
    includeCinematics: true,
    includeOutfits: true,
    streamChunks: Boolean(activeWritable || activeSwPort),
  };

  worker.postMessage({ type: 'start-build', options });
});

if (btnDownloadAgain) {
  btnDownloadAgain.addEventListener('click', () => {
    if (lastBuiltBlob) {
      triggerDownload(lastBuiltBlob);
    }
  });
}
