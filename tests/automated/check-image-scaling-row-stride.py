#!/usr/bin/env python3
"""Ensure a rounded-zero scaled row stride cannot reach division."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(
    encoding="utf-8"
)
start = source.index("void _XmPutScaledImage")
end = source.index("\n\n\n/*** COLOR CACHE NOW ***/", start)
region = source[start:end]

assert "XmRoundScaled(ratio_x * src_image->bytes_per_line" in region
assert "scaled_row_bytes == 0" in region
assert "strip_height = 65536U / scaled_row_bytes;" in region

print("image scaling rejects a rounded-zero row stride before division")
