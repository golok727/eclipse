#!/usr/bin/env python3
from pathlib import Path
import struct
import zlib

OUT = Path(__file__).parent


def png_rgba(path, width, height, pixels):
    raw = bytearray()
    for y in range(height):
        raw.append(0)
        for x in range(width):
            raw.extend(pixels(x, y))

    def chunk(kind, data):
        body = kind + data
        return struct.pack(">I", len(data)) + body + struct.pack(">I", zlib.crc32(body) & 0xFFFFFFFF)

    data = b"\x89PNG\r\n\x1a\n"
    data += chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0))
    data += chunk(b"IDAT", zlib.compress(bytes(raw), 9))
    data += chunk(b"IEND", b"")
    path.write_bytes(data)


def solid(color):
    return lambda x, y: color


def ball(base, edge):
    def px(x, y):
        cx = cy = 7.5
        dx = x - cx
        dy = y - cy
        r2 = dx * dx + dy * dy
        if r2 > 7.2 * 7.2:
            return (0, 0, 0, 0)
        if r2 > 6.0 * 6.0:
            return edge
        if x < 6 and y < 6:
            return (255, 255, 255, 210)
        return base
    return px


def snake_head(x, y):
    green = (80, 240, 140, 255)
    dark = (8, 25, 16, 255)

    # Two 2x2 eyes on the right side of a right-facing 16x16 head.
    upper_eye = x in (11, 12) and y in (3, 4)
    lower_eye = x in (11, 12) and y in (11, 12)
    return dark if upper_eye or lower_eye else green


png_rgba(OUT / "tile_floor.png", 16, 16, solid((32, 36, 45, 255)))
png_rgba(OUT / "tile_wall.png", 16, 16, solid((83, 91, 110, 255)))
png_rgba(OUT / "snake_head.png", 16, 16, snake_head)
png_rgba(OUT / "snake_body.png", 16, 16, solid((40, 180, 100, 255)))
png_rgba(OUT / "reward_red.png", 16, 16, ball((235, 50, 55, 255), (120, 20, 25, 255)))
png_rgba(OUT / "bonus_blue.png", 16, 16, ball((55, 130, 255, 255), (20, 50, 150, 255)))
print("snake assets written")
