// Localization dictionary for Tekken 3 Expanded Web Installer (English).

export const LANGUAGES = [
  { code: 'en', name: 'English', short: 'EN' },
];

let currentLang = 'en';

export function setLanguage(lang) {
  currentLang = 'en';
}

export function getLanguage() {
  return 'en';
}

export function preferredLanguage(saved = null) {
  return 'en';
}

const STRINGS = {
  en: {
    appTitle: "Tekken 3 Expanded — Nintendo Switch Web Installer",
    heroBadge: "Nintendo Switch Native Port",
    subtitle: "Install Tekken 3 Expanded for your Nintendo Switch with 18 TTT1 characters, 28 3D arcade stages, cinematics, outfits and remastered PS2 stage music directly in your Switch",
    dropAreaTitle: "Drop your files here",
    dropAreaSub: "Drop tekken3.zip, tektagt.zip and your Tekken Tag PS2 ISO (or a single .zip / .7z containing them)",
    orBrowse: "or choose files",
    btnChooseFiles: "Select Files",
    btnChooseFolder: "Select Folder",
    detectedComponents: "Required Files:",
    compT3Arcade: "Tekken 3 Arcade ROM (tekken3.zip)",
    compT3ArcadeHint: "Namco System 12 MAME set",
    compTttArcade: "Tekken Tag Arcade ROM (tektagt.zip)",
    compTttArcadeHint: "Namco System 12 MAME set",
    compTttPs2: "Tekken Tag PS2 Disc (SLUS-20001)",
    compTttPs2Hint: "ISO or BIN/CUE image (USA v1.00 or v2.00)",
    statusMissing: "Not detected",
    statusChecking: "Checking...",
    statusDetected: "Detected",
    btnBuild: "Build Switch Package",
    btnBuilding: "Building Package...",
    buildingTitle: "Generating Nintendo Switch Package...",
    buildLog: "Build Log:",
    buildComplete: "Switch Package Built Successfully!",
    buildSuccessMsg: "Your file <strong>tekken3-expanded.zip</strong> has downloaded. Extract it and copy the <code>switch</code> folder directly to the root of your microSD card (<code>sdmc:/</code>).",
    sdCardPreview: "MicroSD Card Structure:",
    errorOccurred: "An error occurred while processing files:",
    fileSize: "Size:",
  }
};

export function t(key) {
  const langTable = STRINGS.en;
  return langTable[key] || key;
}

export function formatSize(bytes) {
  if (bytes === 0) return '0 B';
  const units = ['B', 'KB', 'MB', 'GB'];
  const i = Math.floor(Math.log(bytes) / Math.log(1024));
  return `${(bytes / Math.pow(1024, i)).toFixed(i > 1 ? 1 : 0)} ${units[i]}`;
}
