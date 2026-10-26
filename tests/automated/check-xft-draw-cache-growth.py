#!/usr/bin/env python3
"""Check Xft draw-cache growth is bounded and failure-safe."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/XmRenderT.c").read_text(
    encoding="utf-8"
)

assert "(INT_MAX - 8) / 2" in source
assert "SIZE_MAX / sizeof(*new_cache)" in source
assert "UINT_MAX / sizeof(*new_cache)" in source
assert "if (new_cache == NULL)" in source
assert "XftDrawDestroy(draw);" in source
print("Xft draw-cache growth is bounded and failure-safe")
