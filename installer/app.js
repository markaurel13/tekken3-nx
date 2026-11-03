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

let worker = null;
let inspectedState = null;
let lastBuiltBlob = null;

function triggerDownload(blob, filename = 'tekken3-expanded.zip') {
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = filename;
  document.body.appendChild(a);
  a.click();
  setTimeout(() => {
    try { document.body.removeChild(a); } catch (e) {}
    URL.revokeObjectURL(url);
  }, 60000);
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
  // All 3 files are strictly required to build the full expansion package
  const canBuild = inspectedState.hasT3Arcade && inspectedState.hasTttArcade && inspectedState.hasPs2Disc;
  btnBuild.disabled = !canBuild;
}

// Worker initialization
function initWorker() {
  if (worker) return;
  worker = new Worker('./worker.js', { type: 'module' });

  worker.addEventListener('message', (event) => {
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
      checkBuildReadiness();
      if (btnClear) {
        btnClear.style.display = (data.hasT3Arcade || data.hasTttArcade || data.hasPs2Disc) ? 'inline-flex' : 'none';
      }
    } else if (data.type === 'build-finished') {
      btnBuild.disabled = false;
      btnBuild.textContent = "⚡ Build Switch Package";
      resultCard.hidden = false;
      appendLog("✓ Build complete! Package ready for download.");
      if (data.blob) {
        lastBuiltBlob = data.blob;
        triggerDownload(data.blob);
      }
    } else if (data.type === 'build-error') {
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
