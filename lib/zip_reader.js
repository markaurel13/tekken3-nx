// Fast streaming reader for standard ZIP files via Blob / File.
// Reads End of Central Directory and Central Directory records without loading the full ZIP into memory.
// Uses native DecompressionStream('deflate-raw') supported in all modern browsers.

export async function readZipEntries(blob) {
  const size = blob.size;
  if (size < 22) throw new Error("File too small to be a ZIP archive");

  // Read the last 65KB to locate EOCD record (signature 0x06054b50)
  const searchLen = Math.min(size, 65536 + 22);
  const tailBuffer = await blob.slice(size - searchLen, size).arrayBuffer();
  const tailView = new DataView(tailBuffer);
  const tailBytes = new Uint8Array(tailBuffer);

  let eocdOffset = -1;
  for (let i = tailBytes.length - 22; i >= 0; i--) {
    if (tailView.getUint32(i, true) === 0x06054b50) {
      eocdOffset = size - searchLen + i;
      break;
    }
  }

  if (eocdOffset === -1) {
    throw new Error("Invalid ZIP archive: End of Central Directory signature not found");
  }

  const eocdView = new DataView(tailBuffer, eocdOffset - (size - searchLen), 22);
  const numEntries = eocdView.getUint16(10, true);
  const cdSize = eocdView.getUint32(12, true);
  const cdOffset = eocdView.getUint32(16, true);

  // Read Central Directory
  const cdBuffer = await blob.slice(cdOffset, cdOffset + cdSize).arrayBuffer();
  const cdView = new DataView(cdBuffer);
  let pos = 0;
  const entries = [];
  const textDecoder = new TextDecoder('utf-8');

  while (pos < cdSize) {
    if (cdView.getUint32(pos, true) !== 0x02014b50) break;
    const method = cdView.getUint16(pos + 10, true);
    const crc = cdView.getUint32(pos + 16, true);
    const compressedSize = cdView.getUint32(pos + 20, true);
    const uncompressedSize = cdView.getUint32(pos + 24, true);
    const fileNameLen = cdView.getUint16(pos + 28, true);
    const extraLen = cdView.getUint16(pos + 30, true);
    const commentLen = cdView.getUint16(pos + 32, true);
    const localHeaderOffset = cdView.getUint32(pos + 42, true);

    const nameBytes = new Uint8Array(cdBuffer, pos + 46, fileNameLen);
    const fileName = textDecoder.decode(nameBytes);

    entries.push({
      name: fileName,
      method,
      crc,
      compressedSize,
      uncompressedSize,
      localHeaderOffset,
      async extract() {
        // Read local file header to find exact data start
        const lhBuffer = await blob.slice(localHeaderOffset, localHeaderOffset + 30).arrayBuffer();
        const lhView = new DataView(lhBuffer);
        if (lhView.getUint32(0, true) !== 0x04034b50) {
          throw new Error(`Corrupted local header for ${fileName}`);
        }
        const lhNameLen = lhView.getUint16(26, true);
        const lhExtraLen = lhView.getUint16(28, true);
        const dataOffset = localHeaderOffset + 30 + lhNameLen + lhExtraLen;

        const rawData = blob.slice(dataOffset, dataOffset + compressedSize);
        if (method === 0) {
          // Stored
          return new Uint8Array(await rawData.arrayBuffer());
        } else if (method === 8) {
          // Deflate
          if (typeof DecompressionStream !== 'undefined') {
            const ds = new DecompressionStream('deflate-raw');
            const stream = rawData.stream().pipeThrough(ds);
            const response = new Response(stream);
            return new Uint8Array(await response.arrayBuffer());
          }
          throw new Error("DecompressionStream is not supported in this environment");
        } else {
          throw new Error(`Unsupported ZIP compression method: ${method}`);
        }
      }
    });

    pos += 46 + fileNameLen + extraLen + commentLen;
  }

  return entries;
}
