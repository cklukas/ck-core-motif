#!/usr/bin/env python3
"""Check decoded image fallback backgrounds use a visible screen pixel."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("if (IsSvgFile(file_name))")
end = source.index("#endif", start)
svg_region = source[start:end]
assert "Pixel background = WhitePixelOfScreen(screen);" in svg_region
assert "background = WhitePixelOfScreen(screen);" in svg_region
png_start = source.index("#ifdef PNG_SUPPORTED", end)
png_end = source.index("#endif", png_start)
png_region = source[png_start:png_end]
assert "background = WhitePixelOfScreen(screen);" in png_region
assert "background = 0; /* XXX is if OK? */" not in source
print("SVG and PNG image fallbacks use the screen background")
