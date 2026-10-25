#!/usr/bin/env python3
"""Check optional PushButtonGadget default-glyph geometry is bounds-safe."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "PushBG.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nDrawDefaultGlyphPixmap(\n") + 1
end = text.index("#endif /* DEFAULT_GLYPH_PIXMAP */", start)
body = text[start:end]
for fragment in (
    "int          content_height;",
    "int          right_offset;",
    "(int)pb->rectangle.width <= right_offset",
    "content_height - (int)def_pixmap_height",
    "if (width <= 0 || height <= 0)",
):
    if fragment not in body:
        raise SystemExit("PushButtonGadget glyph guard missing: " + fragment)
erase_start = text.index("\nEraseDefaultGlyphPixmap(\n") + 1
erase_end = text.index("#endif /* DEFAULT_GLYPH_PIXMAP */", erase_start)
if "if ((int)pb->rectangle.width <= right_offset)" not in text[erase_start:erase_end]:
    raise SystemExit("PushButtonGadget glyph erase width guard missing")
print("PushButtonGadget default-glyph geometry guards are present")
