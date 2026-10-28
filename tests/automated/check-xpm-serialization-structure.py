#!/usr/bin/env python3
"""Check all XPM image serializers validate their source structure."""
from pathlib import Path

root = Path(__file__).parents[2]
for name, marker in (
    ("XpmCrDatFrI.c", "XpmCreateDataFromXpmImage("),
    ("XpmCrBufFrI.c", "XpmCreateBufferFromXpmImage("),
    ("XpmWrFFrI.c", "XpmWriteFileFromXpmImage("),
):
    source = (root / "lib" / "Xm" / name).read_text(encoding="utf-8")
    start = source.index(marker)
    body = source[start:source.index("return XpmFileInvalid;", start) + len("return XpmFileInvalid;")]
    for condition in (
        "image->width == 0",
        "image->height == 0",
        "image->cpp == 0",
        "image->ncolors == 0",
        "image->colorTable == NULL",
        "image->data == NULL",
    ):
        assert condition in body, (name, condition)
print("PASS XPM serializers validate image structure")
