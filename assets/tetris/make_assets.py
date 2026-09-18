#!/usr/bin/env python3
from pathlib import Path
import struct
import zlib

OUT = Path(__file__).parent
OUT.mkdir(parents=True, exist_ok=True)

SIZE = 32
LAST = SIZE - 1


def png_rgba(path, width, height, pixels):
    raw = bytearray()
    for y in range(height):
        raw.append(0)
        for x in range(width):
            raw.extend(pixels(x, y))

    def chunk(kind, data):
        body = kind + data
        return (
            struct.pack(">I", len(data))
            + body
            + struct.pack(">I", zlib.crc32(body) & 0xFFFFFFFF)
        )

    data = b"\x89PNG\r\n\x1a\n"
    data += chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0))
    data += chunk(b"IDAT", zlib.compress(bytes(raw), 9))
    data += chunk(b"IEND", b"")
    path.write_bytes(data)


def shade(color, amount, alpha=None):
    """Adjust RGB while always returning exactly one valid RGBA pixel."""
    return tuple(max(0, min(255, channel + amount)) for channel in color[:3]) + (
        color[3] if alpha is None else alpha,
    )


def background(x, y):
    # A quiet star-grid pattern that does not compete with the playfield.
    glow = 5 if (x + y) % 16 == 0 else 0
    return (7 + glow, 10 + glow, 25 + glow, 255)


def grid_cell(x, y):
    if x in (0, LAST) or y in (0, LAST):
        return (29, 37, 68, 255)
    if x in (1, LAST - 1) or y in (1, LAST - 1):
        return (15, 20, 42, 255)
    return (9, 13, 29, 255)


def frame(x, y):
    if x <= 1 or y <= 1:
        return (115, 127, 220, 255)
    if x >= LAST - 1 or y >= LAST - 1:
        return (30, 25, 73, 255)
    if x in (2, LAST - 2) or y in (2, LAST - 2):
        return (75, 65, 155, 255)
    return (48, 42, 105, 255)


def block(base):
    def pixel(x, y):
        # Dark outer outline plus a clean, arcade-style bevel.
        if x in (0, LAST) or y in (0, LAST):
            return shade(base, -105)
        if x in (1, 2) or y in (1, 2):
            return shade(base, 65)
        if x in (LAST - 2, LAST - 1) or y in (LAST - 2, LAST - 1):
            return shade(base, -75)
        if x in (4, LAST - 4) or y in (4, LAST - 4):
            return shade(base, -24)
        if 6 <= x <= 12 and 6 <= y <= 9:
            return shade(base, 34)
        if 7 <= x <= LAST - 7 and 7 <= y <= LAST - 7:
            return shade(base, 8)
        return base

    return pixel


def ghost(x, y):
    if x <= 2 or x >= LAST - 2 or y <= 2 or y >= LAST - 2:
        return (135, 220, 255, 145)
    return (80, 150, 190, 24)


png_rgba(OUT / "background.png", SIZE, SIZE, background)
png_rgba(OUT / "grid.png", SIZE, SIZE, grid_cell)
png_rgba(OUT / "frame.png", SIZE, SIZE, frame)
png_rgba(OUT / "block_i.png", SIZE, SIZE, block((40, 205, 230, 255)))
png_rgba(OUT / "block_o.png", SIZE, SIZE, block((240, 200, 45, 255)))
png_rgba(OUT / "block_t.png", SIZE, SIZE, block((165, 75, 220, 255)))
png_rgba(OUT / "block_s.png", SIZE, SIZE, block((60, 195, 95, 255)))
png_rgba(OUT / "block_z.png", SIZE, SIZE, block((225, 60, 72, 255)))
png_rgba(OUT / "block_j.png", SIZE, SIZE, block((55, 100, 220, 255)))
png_rgba(OUT / "block_l.png", SIZE, SIZE, block((240, 135, 40, 255)))
png_rgba(OUT / "ghost.png", SIZE, SIZE, ghost)
print("tetris assets written")
