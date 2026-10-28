#!/usr/bin/env python3
"""Check image color release has no misleading dead status state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(
    encoding="utf-8"
)
start = source.index("static int\nFreeCacheColors(")
end = source.index("static void\nReleaseImagePixels", start)
region = source[start:end]
assert "(void)closure;" in region
assert "int i, status;" not in region
assert "(void)FreeCacheColor(display, colormap, *pixels);" in region
assert "return 0;" in region
print("PASS image color release callback has explicit best-effort contract")
