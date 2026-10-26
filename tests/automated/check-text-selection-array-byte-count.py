#!/usr/bin/env python3
"""Ensure guarded Text selection-array copies reuse one checked byte count."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
set_start = source.index("static void \nInputSetValues(")
set_end = source.index("\nstatic void \nInputDestroy", set_start)
create_start = source.index("void \n_XmTextInputCreate(")
create_end = source.index("\nstatic XmTextPosition\nXtoPosInLine", create_start)

for region in (source[set_start:set_end], source[create_start:create_end]):
    assert "size_t sarray_bytes;" in region
    assert "sarray_bytes = (size_t)data->sarraycount * sizeof(XmTextScanType);" in region
    assert "(Cardinal)sarray_bytes" in region
    assert "memcpy((void *)" in region
    assert "sarray_bytes);" in region

print("Text selection arrays: checked byte counts are reused for allocation and copy")
