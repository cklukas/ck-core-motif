#!/usr/bin/env python3
"""Ensure IconG mask-name allocation bounds image-name lengths."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c"
text = source.read_text(encoding="utf-8")
start = text.index("/* fetch only if a mask has not been specified")
end = text.index("\n       _XmOSGenerateMaskName", start)
block = text[start:end]

assert "image_name == NULL" in block
assert "image_name_length = strlen(image_name);" in block
assert "image_name_length > SIZE_MAX - 3" in block
assert "image_name_length > (size_t)UINT_MAX - 3" in block
assert "XtMalloc((Cardinal)(image_name_length + 3))" in block

print("IconG mask-name allocation bounds: ok")
