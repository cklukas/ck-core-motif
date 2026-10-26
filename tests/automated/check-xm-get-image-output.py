#!/usr/bin/env python3
"""Check deterministic output handling for the Xm image cache reader."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("static XtEnum\nGetImage(")
end = source.index("\n\x0c\n/*** Keep this one", start)
body = source[start:end]
assert "screen == NULL || image == NULL" in body
assert "*image = NULL;" in body
assert body.index("screen == NULL || image == NULL") < body.index("if (!image_name)")
assert body.index("*image = NULL;") < body.index("if (!image_name)")
print("Xm image-cache output is validated and initialized")
