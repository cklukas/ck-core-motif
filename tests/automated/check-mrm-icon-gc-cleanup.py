#!/usr/bin/env python3
"""Check Mrm icon mappings release partial X resources on GC failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmicon.c").read_text(
    encoding="utf-8"
)

for name, successor in (
    ("Urm__MapIconBitmap", "Urm__MapIconBitmapDepth1"),
    ("Urm__MapIconBitmapDepth1", "Urm__MapIconReplace"),
    ("Urm__MapIconReplace", "Urm__MapIconAllocate"),
    ("Urm__MapIconAllocate", "Urm__RealizeColorTable"),
):
    start = source.index("\n" + name)
    end = source.index("\n" + successor, start + 1)
    region = source[start:end]
    gc_failure = region.index("if ( gc == NULL )")
    error_return = region.index("return Urm__UT_Error", gc_failure)
    cleanup = region[gc_failure:error_return]

    assert "XFreePixmap (display, *pixmap)" in cleanup
    assert "XFree((char*)imagep)" in cleanup
    assert "*pixmap = (Pixmap)0" in cleanup

print("PASS Mrm icon mapping releases X resources after GC setup failure")
