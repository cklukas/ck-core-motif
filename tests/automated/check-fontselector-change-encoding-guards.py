#!/usr/bin/env python3
"""Check FontSelector encoding callback input guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("ChangeEncoding(Widget w")
body = source[start:source.index("/*\tFunction Name: ToggleScaling", start)]

assert "w != NULL && !XtIsSubclass" in body
assert "if (w == NULL)" in body
assert "XmFontS_font_info(fsw) == NULL" in body
assert "ENCODING_LIST(fsw) == NULL" in body
assert "if (encoding_index != 1 || *encodings == NULL)" in body

print("FontSelector encoding callback inputs are guarded")
