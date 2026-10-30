#!/usr/bin/env python3
"""Ensure FileSelectionBox list callbacks validate input and strings."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/FileSB.c").read_text()
start = source.index("ListCallback(\n        Widget wid,")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "if (call_data == NULL || client_data == NULL)" in body
assert "if (change_data.mask != NULL)" in body
assert "if (change_data.dir != NULL)" in body
assert body.index("if (call_data == NULL") < body.index("callback =")
print("FileSelectionBox list callback validates input and strings")
