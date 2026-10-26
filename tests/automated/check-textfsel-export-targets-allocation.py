#!/usr/bin/env python3
"""Check TextField export-target conversion allocation handling."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("if (cs->target == atoms[XmA_MOTIF_EXPORT_TARGETS]")
end = source.index("\n\n  _XmConvertComplete", start)
body = source[start:end]

assert "if (targs == NULL)" in body
assert "_XmConvertComplete(w, NULL, 0, 8, XA_ATOM, cs);" in body
assert "value = (XtPointer) targs;" in body

print("TextField export-target conversion checks allocation")
