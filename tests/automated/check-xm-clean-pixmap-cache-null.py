#!/usr/bin/env python3
"""Check null-safe cleanup of lazily initialized pixmap caches."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("_XmCleanPixmapCache(Screen * screen, Widget shell)")
body = source[start:source.index("/************************************************************************\n*******************************************************************", start)]
assert "if (gc_set != NULL)" in body
assert "if (pixmap_set != NULL)" in body
assert body.index("if (gc_set != NULL)") < body.index("_XmMapHashTable(gc_set")
assert body.index("if (pixmap_set != NULL)") < body.index("_XmMapHashTable(pixmap_set")
print("Xm pixmap-cache cleanup tolerates uninitialized tables")
