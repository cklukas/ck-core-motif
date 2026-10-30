#!/usr/bin/env python3
"""Ensure FileSelectionBox push-button callbacks validate their inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/FileSB.c").read_text()
start = source.index("FileSelectionPB(\n        Widget wid,")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "if (wid == NULL || call_data == NULL)" in body
assert "if (fs == NULL)" in body
assert "if (searchData.mask != NULL)" in body
assert "if (searchData.dir != NULL)" in body
assert body.index("if (wid == NULL") < body.index("fs =")
print("FileSelectionBox push-button callback validates inputs")
