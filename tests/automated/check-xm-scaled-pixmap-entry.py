#!/usr/bin/env python3
"""Check scaled-pixmap cache-entry allocation guards."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("static void\nDiscardScaledPixmapBuild")
end = source.index("/*******************************************************************/\n/* the real one", start)
helper = source[start:end]
body_start = source.index("_XmGetScaledPixmap(")
body = source[body_start:]
assert "Boolean image_lock_held = False;" in body
assert "image_lock_held = True;" in body
assert "if (pix_entry == NULL)" in body
assert "if (pix_entry->acc_color == NULL)" in body
assert "if (pix_entry->image_name == NULL)" in body
assert body.count("DiscardScaledPixmapBuild(ret, image") >= 3
assert "ReleaseImagePixels(screen, pixels, npixels);" in helper
assert "_XmProcessUnlock();" in helper
print("Xm scaled-pixmap cache-entry allocations are guarded")
