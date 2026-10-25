#!/usr/bin/env python3
"""Check optional PushButton default-glyph geometry is bounds-safe."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "PushB.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nDrawDefaultGlyphPixmap(\n") + 1
end = text.index("#endif /* DEFAULT_GLYPH_PIXMAP */", start)
body = text[start:end]
required = [
    "int content_height;",
    "int right_offset;",
    "(int)pb->core.width <= right_offset",
    "content_height - (int)def_pixmap_height",
    "if (width <= 0 || height <= 0)",
]
missing = [fragment for fragment in required if fragment not in body]
if missing:
    raise SystemExit("PushButton glyph bounds guard missing: " + ", ".join(missing))
erase_start = text.index("\nEraseDefaultGlyphPixmap(\n") + 1
erase_end = text.index("#endif /* DEFAULT_GLYPH_PIXMAP */", erase_start)
erase = text[erase_start:erase_end]
if "if ((int)pb->core.width <= right_offset)" not in erase:
    raise SystemExit("PushButton glyph erase width guard missing")
print("PushButton default-glyph geometry guards are present")
