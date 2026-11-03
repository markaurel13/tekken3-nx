// Service Worker for streaming download and offline caching.
// Adapted from nfsmw-nx-installer by StevensND (GPL-3.0) and defjam-sw-installer.

const PAGE_CACHE = 'tekken3-expanded-page-v1';

const PAGE_FILES = [
  './',
  'index.html',
  'app.js',
  'worker.js',
  'lib/i18n.js',
  'lib/installer.js',
  'lib/iso.js',
  'lib/zip.js',
  'lib/zip_reader.js',
  'lib/arcade.js',
  'lib/ps2.js',
  'release/manifest.json'
];

const jobs = new Map();

self.addEventListener('install', () => self.skipWaiting());

self.addEventListener('activate', (event) =>
  event.waitUntil(
    (async () => {
      for (const name of await caches.keys()) {
        if (name !== PAGE_CACHE) {
          await caches.delete(name);
        }
      }
      await self.clients.claim();
    })()
  )
);

self.addEventListener('message', (event) => {
  const data = event.data || {};
  if (data.type === 'download') {
    const [port, ack] = event.ports;
    jobs.set(data.id, { port, size: data.size, filename: data.filename });
    ack.postMessage('registered');
  } else if (data.type === 'claim') {
    event.waitUntil(self.clients.claim());
  }
});

self.addEventListener('fetch', (event) => {
  const request = event.request;
  const url = new URL(request.url);

  // Download stream interception
  const match = /\/download\/([\w-]+)\/[^/]+$/.exec(url.pathname);
  if (match) {
    const job = jobs.get(match[1]);
    if (job) {
      jobs.delete(match[1]);
      event.respondWith(serveDownload(job));
    }
    return;
  }

  // Bypass non-GET or cross-origin
  if (request.method !== 'GET' || !url.protocol.startsWith('http')) {
    return;
  }
});

function serveDownload(job) {
  const { port } = job;
  const stream = new ReadableStream(
    {
      start(controller) {
        port.onmessage = ({ data }) => {
          if (data.type === 'chunk') {
            controller.enqueue(new Uint8Array(data.buffer));
          } else if (data.type === 'end') {
            controller.close();
          } else if (data.type === 'error') {
            controller.error(new Error(data.message));
          }
        };
      },
      pull() {
        port.postMessage({ type: 'pull' });
      },
      cancel() {
        port.postMessage({ type: 'cancel' });
      },
    },
    { highWaterMark: 4 }
  );

  const headers = {
    'Content-Type': 'application/zip',
    'Content-Disposition': `attachment; filename="${job.filename}"`,
  };
  if (job.size) {
    headers['Content-Length'] = String(job.size);
  }

  return new Response(stream, { headers });
}
