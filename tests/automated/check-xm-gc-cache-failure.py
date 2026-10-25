#!/usr/bin/env python3
"""Check failure handling for the Xm image-to-pixmap GC cache."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("static GC \nGetGCForPutImage(")
end = source.index("/************", start)
body = source[start:end]
assert "if (gc_set == NULL)" in body
assert "return XCreateGC" in body
assert "if (gc_entry == NULL)" in body
assert "if (gc_entry->gc == NULL)" in body
assert body.index("gc_entry->gc = XCreateGC") < body.index("_XmAddHashEntry(gc_set")
print("Xm GC cache failures fall back without publishing invalid entries")
