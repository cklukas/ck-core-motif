#!/usr/bin/env python3
"""Check deterministic output handling for the Xm image cache reader."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("static XtEnum\nGetImage(")
end = source.index("\n\x0c\n/*** Keep this one", start)
body = source[start:end]
if "if (image == NULL)" not in body or "*image = NULL;" not in body:
    raise SystemExit("GetImage does not initialize its output")
if body.index("if (image == NULL)") > body.index("if (!image_name)"):
    raise SystemExit("GetImage validates image output too late")
if body.index("*image = NULL;") > body.index("if (!image_name)"):
    raise SystemExit("GetImage clears image output too late")
print("PASS Xm image-cache output is validated and initialized")
