// Multi-language localization dictionary (Spanish and English).

export const LANGUAGES = [
  { code: 'es', name: 'Español', short: 'ES' },
  { code: 'en', name: 'English', short: 'EN' },
];

let currentLang = 'es';

export function setLanguage(lang) {
  if (LANGUAGES.some(l => l.code === lang)) {
    currentLang = lang;
  }
}

export function getLanguage() {
  return currentLang;
}

export function preferredLanguage(saved = null) {
  if (saved && LANGUAGES.some(l => l.code === saved)) return saved;
  const nav = (navigator.language || '').slice(0, 2).toLowerCase();
  return nav === 'es' ? 'es' : 'en';
}

const STRINGS = {
  es: {
    appTitle: "Tekken 3 Expanded — Instalador para Nintendo Switch",
    heroBadge: "Nintendo Switch Port",
    subtitle: "Crea el paquete completo para Switch con luchadores de TTT1, escenarios arcade, cinemáticas, trajes y música remasterizada a partir de tus propios archivos.",
    dropAreaTitle: "Arrastra tus archivos aquí",
    dropAreaSub: "Suelta los archivos directamente o un archivo .zip / .7z con ellos",
    orBrowse: "o también puedes",
    btnChooseFiles: "Seleccionar archivos",
    btnChooseFolder: "Seleccionar carpeta",
    detectedComponents: "Archivos necesarios:",
    compT3Arcade: "Tekken 3 Arcade ROM (tekken3.zip)",
    compTttArcade: "Tekken Tag Arcade ROM (tektagt.zip)",
    compTttPs2: "Tekken Tag PS2 Disc (SLUS-20001)",
    compT3Ps1: "Tekken 3 PS1 Disc (SLUS-00402)",
    statusMissing: "No detectado",
    statusChecking: "Verificando...",
    statusDetected: "Detectado correctamente",
    statusOptional: "Opcional (si ya lo instalaste antes)",
    optionsTitle: "Opciones del paquete:",
    optPs2Music: "Música de escenarios remasterizada de PS2 (ps2-music.ttp)",
    optStages: "Escenarios 3D Arcade & TTT (28 escenarios .ast)",
    optCinematics: "Finales y cinemáticas completas (TTT Endings & Embu)",
    optOutfits: "Menú de trajes alternativos y galería HD",
    optNro: "Incluir ejecutable de Switch actualizado (tekken3_nx.nro)",
    btnBuild: "Crear paquete para Switch",
    btnBuilding: "Construyendo paquete...",
    buildingTitle: "Generando paquete para Switch...",
    buildLog: "Registro de operaciones:",
    buildComplete: "¡Paquete generado con éxito!",
    buildSuccessMsg: "Tu archivo <strong>tekken3-expanded.zip</strong> se ha descargado. Descomprímelo y copia la carpeta <code>switch</code> directamente en la raíz de tu tarjeta microSD (<code>sdmc:/</code>).",
    sdCardPreview: "Estructura en tu tarjeta microSD:",
    errorOccurred: "Ocurrió un error al procesar los archivos:",
    supportedDiscNote: "Compatible con discos de PS2 Tekken Tag Tournament (USA SLUS-20001 v1.00 / v2.00) en formato .iso o .bin/.cue.",
    fileSize: "Tamaño:",
  },
  en: {
    appTitle: "Tekken 3 Expanded — Nintendo Switch Web Installer",
    heroBadge: "Nintendo Switch Port",
    subtitle: "Build the complete Nintendo Switch package with TTT1 characters, arcade 3D stages, cinematics, outfits and remixed music from your own files.",
    dropAreaTitle: "Drag your files here",
    dropAreaSub: "Drop files directly or a .zip / .7z archive containing them",
    orBrowse: "or you can",
    btnChooseFiles: "Choose Files",
    btnChooseFolder: "Choose Folder",
    detectedComponents: "Required files:",
    compT3Arcade: "Tekken 3 Arcade ROM (tekken3.zip)",
    compTttArcade: "Tekken Tag Arcade ROM (tektagt.zip)",
    compTttPs2: "Tekken Tag PS2 Disc (SLUS-20001)",
    compT3Ps1: "Tekken 3 PS1 Disc (SLUS-00402)",
    statusMissing: "Not detected",
    statusChecking: "Checking...",
    statusDetected: "Successfully detected",
    statusOptional: "Optional (if installed previously)",
    optionsTitle: "Package options:",
    optPs2Music: "Remastered PS2 Stage Music (ps2-music.ttp)",
    optStages: "3D Arcade & TTT Stages (28 .ast stages)",
    optCinematics: "Full Endings & Cinematics (TTT Endings & Embu)",
    optOutfits: "Alternate Outfits Menu & HD Gallery",
    optNro: "Include updated Switch binary (tekken3_nx.nro)",
    btnBuild: "Build Switch Package",
    btnBuilding: "Building package...",
    buildingTitle: "Generating Switch Package...",
    buildLog: "Operation log:",
    buildComplete: "Package built successfully!",
    buildSuccessMsg: "Your file <strong>tekken3-expanded.zip</strong> has downloaded. Extract it and copy the <code>switch</code> folder directly to the root of your microSD card (<code>sdmc:/</code>).",
    sdCardPreview: "MicroSD Card Structure:",
    errorOccurred: "An error occurred while processing files:",
    supportedDiscNote: "Compatible with Tekken Tag Tournament (USA SLUS-20001 v1.00 / v2.00) PS2 disc images (.iso or .bin/.cue).",
    fileSize: "Size:",
  }
};

export function t(key) {
  const langTable = STRINGS[currentLang] || STRINGS.es;
  return langTable[key] || STRINGS.en[key] || key;
}

export function formatSize(bytes) {
  if (bytes === 0) return '0 B';
  const units = ['B', 'KB', 'MB', 'GB'];
  const i = Math.floor(Math.log(bytes) / Math.log(1024));
  return `${(bytes / Math.pow(1024, i)).toFixed(i > 1 ? 1 : 0)} ${units[i]}`;
}
