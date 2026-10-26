#!/usr/bin/env python3
"""Ensure TextField selection-array copying is failure-safe."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c"
text = source.read_text(encoding="utf-8")
start = text.index("/*\n   * Fix for HaL DTS 9841")
end = text.index("\n  /*\n   * End fix for HaL DTS 9841", start)
block = text[start:end]

assert "selection_bytes" in block
assert "SIZE_MAX / sizeof(XmTextScanType)" in block
assert "UINT_MAX / sizeof(XmTextScanType)" in block
assert "selection_copy = (XmTextScanType *)XtMalloc" in block
assert "if (selection_copy == NULL)" in block
assert "memcpy((void *)selection_copy" in block

print("TextField selection-array allocation guard: ok")
