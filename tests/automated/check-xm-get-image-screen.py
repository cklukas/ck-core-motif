#!/usr/bin/env python3
"""Check screen validation in the Xm image-cache reader."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("static XtEnum\nGetImage(")
end = source.index("\n\x0c\n/*** Keep this one", start)
body = source[start:end]
needle = "if (screen == NULL || image == NULL)"
assert needle in body
assert body.index(needle) < body.index("DisplayOfScreen(screen)")
assert body.index("*image = NULL;") < body.index("DisplayOfScreen(screen)")
print("Xm image-cache reader validates its screen before display access")
