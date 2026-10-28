#!/usr/bin/env python3
"""Check XPM serializers retain the one-row pixel path."""
from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
for name in ("XpmCrDatFrI.c", "XpmCrBufFrI.c"):
    source = (root / name).read_text()
    function_name = "CreatePixels" if name == "XpmCrDatFrI.c" else "WritePixels"
    start = source.index("static int\n" + function_name + "(")
    start = source.index("{", start)
    end = source.index("\n}", start)
    function = source[start:end]
    assert "if (width == 0 || height == 0)" in function
    assert "height <= 1" not in function
print("PASS XPM one-row pixel serialization")
