// Main User Interface Controller for Tekken 3 Expanded Web Installer.
// Handles Drag & Drop, file inspections, visual status badges, and Service Worker download coordination.

import { LANGUAGES, setLanguage, getLanguage, preferredLanguage, t, formatSize } from './lib/i18n.js';

const dropZone = document.getElementById('dropZone');
const fileInput = document.getElementById('fileInput');
const folderInput = document.getElementById('folderInput');
const btnBuild = document.getElementById('btnBuild');
const progressContainer = document.getElementById('progressContainer');
const progressBar = document.getElementById('progressBar');
const logBox = document.getElementById('logBox');
const resultCard = document.getElementById('resultCard');
const languagesContainer = document.getElementById('languages');

// Badges
const badgeT3Arcade = document.getElementById('badgeT3Arcade');
const badgeTttArcade = document.getElementById('badgeTttArcade');
const badgeTttPs2 = document.getElementById('badgeTttPs2');
const badgeT3Ps1 = document.getElementById('badgeT3Ps1');

// Options
const optPs2Music = document.getElementById('optPs2Music');
const optStages = document.getElementById('optStages');
const optCinematics = document.getElementById('optCinematics');
const optOutfits = document.getElementById('optOutfits');
const optNro = document.getElementById('optNro');

let worker = null;
let downloadHelper = null;
let inspectedState = null;

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
    badgeEl.textContent = `✓ ${t('statusDetected')} ${detail ? `(${detail})` : ''}`;
  } else if (state === 'checking') {
    badgeEl.textContent = `⏳ ${t('statusChecking')}`;
  } else if (state === 'optional') {
    badgeEl.textContent = `○ ${t('statusOptional')}`;
  } else {
    badgeEl.textContent = `✗ ${t('statusMissing')}`;
  }
}

function checkBuildReadiness() {
  if (!inspectedState) {
    btnBuild.disabled = true;
    return;
  }
  // User needs either both arcade zips, or PS2 disc, or all
  const canBuild = (inspectedState.hasT3Arcade && inspectedState.hasTttArcade) || inspectedState.hasPs2Disc;
  btnBuild.disabled = !canBuild;
}

// Language initialization
function applyLanguage(code) {
  setLanguage(code);
  document.documentElement.lang = getLanguage();

  for (const el of document.querySelectorAll('[data-i18n]')) {
    el.textContent = t(el.dataset.i18n);
  }
  for (const el of document.querySelectorAll('[data-i18n-html]')) {
    el.innerHTML = t(el.dataset.i18nHtml);
  }
  for (const btn of languagesContainer.querySelectorAll('button')) {
    btn.classList.toggle('active', btn.dataset.lang === getLanguage());
  }

  // Refresh badges text
  if (inspectedState) {
    updateBadge(badgeT3Arcade, inspectedState.hasT3Arcade ? 'good' : 'bad', inspectedState.t3ArcadeName);
    updateBadge(badgeTttArcade, inspectedState.hasTttArcade ? 'good' : 'bad', inspectedState.tttArcadeName);
    updateBadge(badgeTttPs2, inspectedState.hasPs2Disc ? 'good' : 'bad', inspectedState.ps2DiscName);
    updateBadge(badgeT3Ps1, inspectedState.hasPs1Disc ? 'good' : 'optional');
  } else {
    updateBadge(badgeT3Arcade, 'bad');
    updateBadge(badgeTttArcade, 'bad');
    updateBadge(badgeTttPs2, 'bad');
    updateBadge(badgeT3Ps1, 'optional');
  }
}

for (const { code, name, short } of LANGUAGES) {
  const btn = document.createElement('button');
  btn.type = 'button';
  btn.dataset.lang = code;
  btn.textContent = short;
  btn.title = name;
  btn.addEventListener('click', () => {
    applyLanguage(code);
    try { localStorage.setItem('tekken3.lang', code); } catch (e) {}
  });
  languagesContainer.appendChild(btn);
}

let savedLang = null;
try { savedLang = localStorage.getItem('tekken3.lang'); } catch (e) {}
applyLanguage(preferredLanguage(savedLang));

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
      updateBadge(badgeT3Ps1, data.hasPs1Disc ? 'good' : 'optional');
      checkBuildReadiness();
    } else if (data.type === 'build-finished') {
      btnBuild.disabled = false;
      btnBuild.textContent = t('btnBuild');
      resultCard.hidden = false;
      appendLog(t('buildComplete'));
    } else if (data.type === 'build-error') {
      btnBuild.disabled = false;
      btnBuild.textContent = t('btnBuild');
      appendLog(data.message, true);
    } else if (data.type === 'fallback-download') {
      // Direct blob fallback if service worker streaming unavailable
      const url = URL.createObjectURL(data.blob);
      const a = document.createElement('a');
      a.href = url;
      a.download = 'tekken3-expanded.zip';
      a.click();
      setTimeout(() => URL.revokeObjectURL(url), 60000);
    }
  });
}
initWorker();

// Service Worker Registration for streaming downloads
async function initServiceWorker() {
  if (!('serviceWorker' in navigator)) return null;
  try {
    const reg = await navigator.serviceWorker.register('./sw.js');
    await navigator.serviceWorker.ready;
    if (!navigator.serviceWorker.controller) {
      reg.active?.postMessage({ type: 'claim' });
    }
    return navigator.serviceWorker.controller;
  } catch (err) {
    console.warn("ServiceWorker not registered, falling back to in-memory blob streaming:", err);
    return null;
  }
}
downloadHelper = initServiceWorker();

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
    updateBadge(badgeT3Arcade, 'checking');
    updateBadge(badgeTttArcade, 'checking');
    updateBadge(badgeTttPs2, 'checking');
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
});

folderInput.addEventListener('change', (e) => {
  if (e.target.files.length) handleDroppedItems([...e.target.files]);
});

// Build button click
btnBuild.addEventListener('click', async () => {
  btnBuild.disabled = true;
  btnBuild.textContent = t('btnBuilding');
  progressContainer.hidden = false;
  progressBar.style.width = '0%';
  progressBar.textContent = '0%';
  resultCard.hidden = true;
  logBox.innerHTML = '';

  const options = {
    includePs2Music: optPs2Music.checked,
    includeStages: optStages.checked,
    includeCinematics: optCinematics.checked,
    includeOutfits: optOutfits.checked,
    includeNro: optNro.checked,
  };

  const swController = await downloadHelper;
  if (swController) {
    const channel = new MessageChannel();
    const ack = new MessageChannel();
    const jobId = `tekken3-${Date.now()}`;
    const filename = 'tekken3-expanded.zip';

    // Register job with Service Worker
    swController.postMessage(
      { type: 'download', id: jobId, filename, size: 0 },
      [channel.port2, ack.port1]
    );

    // Wait for ack
    await new Promise(resolve => { ack.port2.onmessage = () => resolve(); });

    // Open iframe to trigger browser download
    const iframe = document.createElement('iframe');
    iframe.hidden = true;
    iframe.src = `./download/${jobId}/${filename}`;
    document.body.appendChild(iframe);

    // Tell worker to start and stream into channel.port1
    worker.postMessage({ type: 'start-build', options }, [channel.port1]);
  } else {
    // Fallback mode without service worker
    worker.postMessage({ type: 'start-build', options });
  }
});
