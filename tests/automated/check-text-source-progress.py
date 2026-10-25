#!/usr/bin/env python3
"""Check that Text source extraction rejects non-progressing reads."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("_XmStringSourceGetString(")
body = source[start:source.index("_XmTextValidate(", start)]
assert body.count("block.length = 0;") >= 2
assert body.count("<= start_pos || block.length == 0") == 2
assert "start_pos = pos;" in body
print("PASS Text source extraction requires forward progress")
