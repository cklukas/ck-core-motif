#!/usr/bin/env python3
"""Ensure cut-buffer context allocation is checked before initialization."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("\n_XmCreateCutBuffers(Widget w)") + 1
end = text.index("\n/****************************************************************", start)
block = text[start:end]

assert "ctx_data = (XmTextContextData) XtMalloc" in block
assert "if (ctx_data == NULL)" in block
assert "return;" in block
assert "ctx_data->screen = screen;" in block

print("text cut-buffer context allocation guard: ok")
