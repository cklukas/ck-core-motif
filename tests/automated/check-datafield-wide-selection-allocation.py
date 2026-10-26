#!/usr/bin/env python3
"""Ensure DataField wide-selection allocation is failure-safe."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c"
text = source.read_text(encoding="utf-8")
start = text.index("XmDataFieldGetSelectionWcs(")
end = text.index("\n/*", start)
block = text[start:end]

assert "length + 1 > UINT_MAX / sizeof(wchar_t)" in block
assert "wc_value = (wchar_t*)XtMalloc" in block
assert "if (wc_value == NULL)" in block
assert "return NULL;" in block

print("DataField wide-selection allocation guard: ok")
