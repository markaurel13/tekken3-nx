#!/usr/bin/env python3
"""Local test server for Tekken 3 Expanded Web Installer.
Serves files with correct MIME types for Service Worker, WASM, and JS modules.
"""
import functools
import http.server
import os
import sys

TYPES = {
    '.mjs': 'text/javascript',
    '.js': 'text/javascript',
    '.wasm': 'application/wasm',
    '.json': 'application/json',
    '.ast': 'application/octet-stream',
    '.ttp': 'application/octet-stream',
    '.ttm': 'application/octet-stream',
    '.tim': 'application/octet-stream',
    '.pcm': 'application/octet-stream',
    '.cine': 'application/octet-stream',
    '.nro': 'application/octet-stream',
    '.rgba': 'application/octet-stream',
}

class Handler(http.server.SimpleHTTPRequestHandler):
    extensions_map = {**http.server.SimpleHTTPRequestHandler.extensions_map, **TYPES}

    def end_headers(self):
        # Enable COOP / COEP for SharedArrayBuffer / high-performance streaming if needed
        self.send_header('Cache-Control', 'no-store')
        self.send_header('Access-Control-Allow-Origin', '*')
        super().end_headers()

if __name__ == '__main__':
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 8196
    root = sys.argv[2] if len(sys.argv) > 2 else os.path.dirname(os.path.abspath(__file__))
    handler = functools.partial(Handler, directory=root)
    print(f"[*] Tekken 3 Expanded Installer test server running at http://127.0.0.1:{port}/")
    print(f"[*] Serving files from: {root}")
    try:
        http.server.ThreadingHTTPServer(('127.0.0.1', port), handler).serve_forever()
    except KeyboardInterrupt:
        print("\n[*] Server stopped.")
