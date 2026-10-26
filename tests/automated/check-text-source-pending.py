#!/usr/bin/env python3
"""Ensure pending-state snapshots check count and allocation bounds."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c"
text = source.read_text(encoding="utf-8")
start = text.index("Boolean *\n_XmStringSourceGetPending")
end = text.index("\nvoid\n_XmStringSourceSetPending", start)
block = text[start:end]

assert "data->numwidgets <= 0" in block
assert "UINT_MAX / sizeof(Boolean)" in block
assert "pending = (Boolean *)XtMalloc" in block
assert "if (pending == NULL)" in block
assert "return NULL;" in block

print("text-source pending snapshot guards: ok")
