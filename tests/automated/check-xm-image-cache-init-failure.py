#!/usr/bin/env python3
"""Check image-cache initialization failure handling."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("static XtEnum\nGetImage(")
end = source.index("\n\x0c\n/*** Keep this one", start)
body = source[start:end]
needle = "if (image_set == NULL) InitializeImageSet();"
assert needle in body
assert "if (image_set == NULL)\n        return FALSE;" in body
assert body.index("if (image_set == NULL)\n        return FALSE;") > body.index(needle)
assert body.index("if (image_set == NULL)\n        return FALSE;") < body.index("_XmGetHashEntry(image_set")
print("Xm image-cache initialization failures return safely")
