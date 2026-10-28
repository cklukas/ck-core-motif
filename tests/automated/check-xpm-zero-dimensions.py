#!/usr/bin/env python3
"""Check XPM array and buffer pixel writers reject zero dimensions."""
from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
for name, function_name in (("XpmCrDatFrI.c", "CreatePixels"),
                            ("XpmCrBufFrI.c", "WritePixels")):
    source = (root / name).read_text()
    start = source.index("static int\n" + function_name + "(")
    start = source.index("{", start)
    end = source.index("\n}", start)
    function = source[start:end]
    assert "if (width == 0 || height == 0)" in function
    assert "return 0;" in function
print("PASS XPM zero-dimension rejection")
