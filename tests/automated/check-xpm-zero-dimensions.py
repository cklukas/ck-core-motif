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

buffer_source = (root / "XpmCrBufFrI.c").read_text()
entry = buffer_source.index("XpmCreateBufferFromXpmImage(\n")
entry = buffer_source.index("{", entry)
entry_end = buffer_source.index("\n}", entry)
entry_function = buffer_source[entry:entry_end]
guard = "buffer_return == NULL || image == NULL ||\n\timage->width == 0 || image->height == 0"
assert guard in entry_function
