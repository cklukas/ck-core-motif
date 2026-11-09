#!/usr/bin/env python3
"""Check JPEG dimensions remain representable by the XImage consumer."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Jpeg.c").read_text(
    encoding="utf-8"
)

assert "JDIMENSION x, y;" in source
assert "for (x = cinfo.output_width; x > 0; --x)" in source
assert "buf[x - 1].red = buf[x - 1].green = buf[x - 1].blue" in source
assert "image_width > INT_MAX" in source
assert "image_height > INT_MAX" in source
assert "if (screen->root_depth <= 0)" in source
assert "(unsigned int)screen->root_depth" in source
assert "(unsigned int)image_width" in source
assert "(unsigned int)image_height" in source

print("PASS JPEG decoder bounds dimensions before XImage construction")
