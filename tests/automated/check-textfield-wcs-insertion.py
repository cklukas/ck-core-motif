#!/usr/bin/env python3
"""Check TextField wide insertion conversion sizes and allocation."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("InsertSelection(")
end = source.index("\nstatic void\nHandleInsertTargets", start)
body = source[start:end]
assert "size_t len = strlen(total_value) + 1;" in body
assert "len > (size_t)INT_MAX" in body
assert "if (wc_value == NULL)" in body
assert "XtMalloc((Cardinal)((size_t)len * sizeof(wchar_t)))" in body

print("TextField wide insertion conversion is bounded")
