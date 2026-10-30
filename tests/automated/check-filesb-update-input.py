#!/usr/bin/env python3
"""Ensure FileSelectionBoxUpdate validates its widget and search data."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/FileSB.c").read_text()
start = source.index("FileSelectionBoxUpdate(\n        XmFileSelectionBoxWidget fs,")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "if (fs == NULL || searchData == NULL)" in body
assert body.index("if (fs == NULL") < body.index("XtSetMappedWhenManaged")
print("FileSelectionBoxUpdate validates widget and search data")
