#!/usr/bin/env python3
"""Check that DataField diagnostics, initialization, and getters handle OOM."""

from pathlib import Path


source = Path("lib/Xm/DataF.c").read_text(encoding="utf-8")

assert source.count("if (message == NULL)") >= 2
assert "if (XmTextF_highlight(tf).list != NULL)" in source
assert source.count("if (temp_wcs == NULL)") >= 2
assert source.count("if (value == NULL)") >= 2
assert "if (base_array == NULL)" in source

print("DataField getter allocation checks passed")
