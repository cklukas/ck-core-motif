#!/usr/bin/env python3
"""Check TextField insertion conversion bounds external lengths."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("InsertSelection(")
end = source.index("\nstatic void\nHandleInsertTargets", start)
body = source[start:end]
assert "*length > (unsigned long)INT_MAX" in body
assert "*length > (unsigned long)UINT_MAX / sizeof(wchar_t) - 1" in body
assert body.count("if (temp == NULL)") >= 1
assert body.count("if (wc_value == NULL)") >= 1

print("TextField insertion conversion bounds external lengths")
