// ISO 9660 and Raw Disc Image Sector Reader
// Allows random-access streaming across large 2GB - 4GB ISO/BIN images without loading them into RAM.

export const SECTOR_SIZE_ISO = 2048;
export const SECTOR_SIZE_RAW = 2352;

export class DiscReader {
  constructor(blob) {
    this.blob = blob;
    this.sectorSize = SECTOR_SIZE_ISO;
    this.dataOffset = 0;
    this.ready = false;
  }

  async init() {
    if (this.ready) return;
    if (this.blob.size < 16 * SECTOR_SIZE_ISO) {
      throw new Error("Disc image is too small to be a valid CD/DVD");
    }

    // Inspect the first 16 bytes for RAW sync marker
    const head = new Uint8Array(await this.blob.slice(0, 16).arrayBuffer());
    const isRaw = (
      head[0] === 0x00 &&
      head[1] === 0xFF && head[2] === 0xFF && head[3] === 0xFF &&
      head[4] === 0xFF && head[5] === 0xFF && head[6] === 0xFF &&
      head[7] === 0xFF && head[8] === 0xFF && head[9] === 0xFF &&
      head[10] === 0xFF && head[11] === 0x00
    );

    if (isRaw) {
      this.sectorSize = SECTOR_SIZE_RAW;
      this.dataOffset = 24; // Standard Mode 2 Form 1 data offset
    } else {
      this.sectorSize = SECTOR_SIZE_ISO;
      this.dataOffset = 0;
    }

    this.ready = true;
  }

  // Reads `count` consecutive 2048-byte user data sectors starting from `lba`
  async readSectors(lba, count) {
    await this.init();
    const result = new Uint8Array(count * 2048);
    let outOffset = 0;

    // Batch read in chunks for high throughput (e.g. up to 1024 sectors = 2MB per slice)
    const BATCH = 512;
    for (let current = lba; current < lba + count; current += BATCH) {
      const currentBatch = Math.min(BATCH, (lba + count) - current);
      if (this.sectorSize === 2048 && this.dataOffset === 0) {
        // Direct read
        const byteStart = current * 2048;
        const byteEnd = byteStart + currentBatch * 2048;
        const chunk = new Uint8Array(await this.blob.slice(byteStart, byteEnd).arrayBuffer());
        result.set(chunk, outOffset);
        outOffset += chunk.length;
      } else {
        // Raw sector stripping
        const byteStart = current * this.sectorSize;
        const byteEnd = (current + currentBatch) * this.sectorSize;
        const rawChunk = new Uint8Array(await this.blob.slice(byteStart, byteEnd).arrayBuffer());
        for (let s = 0; s < currentBatch; s++) {
          const sOffset = s * this.sectorSize + this.dataOffset;
          result.set(rawChunk.subarray(sOffset, sOffset + 2048), outOffset);
          outOffset += 2048;
        }
      }
    }

    return result;
  }

  // Checks if sector 16 contains standard ISO 9660 PVD marker ('CD001')
  async verifyIso9660() {
    await this.init();
    const pvd = await this.readSectors(16, 1);
    const magic = String.fromCharCode(...pvd.subarray(1, 6));
    return magic === 'CD001';
  }

  // Reads the Root Directory table to list top-level files
  async listFiles() {
    await this.init();
    const pvd = await this.readSectors(16, 1);
    if (String.fromCharCode(...pvd.subarray(1, 6)) !== 'CD001') {
      return [];
    }

    // Root directory record is at PVD offset 156
    const rootRecord = pvd.subarray(156, 156 + 34);
    const rootLba = rootRecord[2] | (rootRecord[3] << 8) | (rootRecord[4] << 16) | (rootRecord[5] << 24);
    const rootSize = rootRecord[10] | (rootRecord[11] << 8) | (rootRecord[12] << 16) | (rootRecord[13] << 24);

    const numSectors = Math.ceil(rootSize / 2048);
    const dirBytes = await this.readSectors(rootLba, numSectors);

    const files = [];
    let offset = 0;
    while (offset < rootSize) {
      const len = dirBytes[offset];
      if (len === 0) {
        // Sector padding, advance to next sector
        offset = (Math.floor(offset / 2048) + 1) * 2048;
        if (offset >= rootSize) break;
        continue;
      }

      const fileLba = dirBytes[offset + 2] | (dirBytes[offset + 3] << 8) | (dirBytes[offset + 4] << 16) | (dirBytes[offset + 5] << 24);
      const fileSize = dirBytes[offset + 10] | (dirBytes[offset + 11] << 8) | (dirBytes[offset + 12] << 16) | (dirBytes[offset + 13] << 24);
      const flags = dirBytes[offset + 25];
      const nameLen = dirBytes[offset + 32];
      let name = '';
      for (let i = 0; i < nameLen; i++) {
        name += String.fromCharCode(dirBytes[offset + 33 + i]);
      }
      // Remove ISO version suffix ';1'
      name = name.split(';')[0];

      if (name && name !== '\x00' && name !== '\x01') {
        files.push({
          name,
          lba: fileLba,
          size: fileSize,
          isDirectory: (flags & 2) !== 0,
        });
      }

      offset += len;
    }

    return files;
  }
}
