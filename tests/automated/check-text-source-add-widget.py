#!/usr/bin/env python3
"""Ensure text-source widget attachment handles realloc failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void \nAddWidget")
end = text.index("\n/********************************<->***********************************/", start)
block = text[start:end]

assert "new_widgets = (XmTextWidget *)XtRealloc" in block
assert "if (new_widgets == NULL)" in block
assert "data->numwidgets--;" in block
assert "new_highlights = (_XmHighlightRec *)XtRealloc" in block
assert "if (new_highlights == NULL)" in block
assert "UINT_MAX / sizeof(XmTextWidget)" in block

print("text-source AddWidget allocation guards: ok")
