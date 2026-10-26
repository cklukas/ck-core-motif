#!/usr/bin/env python3
"""Ensure TextField size calculations cannot wrap legacy dimensions."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text(
    encoding="utf-8"
)
start = source.index("static void \nComputeSize(XmTextFieldWidget tf,")
end = source.index("\nstatic XtGeometryResult \nTryResize", start)
region = source[start:end]

assert "uint64_t horizontal" in region
assert "uint64_t vertical" in region
assert "uint64_t text_width" in region
assert "TextFieldBoundDimension(text_width + horizontal)" in region
assert "TextFieldBoundDimension(" in region
assert "TextF_Columns(tf) * tf->text.average_char_width" not in region

print("TextField size arithmetic is widened and dimension-bounded")
