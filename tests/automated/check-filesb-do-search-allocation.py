#!/usr/bin/env python3
"""Ensure XmFileSelectionDoSearch handles compound-string allocation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/FileSB.c").read_text()
start = source.index("XmFileSelectionDoSearch(\n        Widget fs,")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert body.count("if (searchData.mask != NULL)") == 2
assert "if (searchData.dir != NULL)" in body
assert body.index("if (searchData.mask != NULL)") < body.index("FileSelectionBoxUpdate")
print("XmFileSelectionDoSearch handles compound-string allocation failure")
