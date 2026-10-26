#!/usr/bin/env python3
"""Ensure text selection target-list allocation failure is reported safely."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c"
text = source.read_text(encoding="utf-8")
start = text.index("  if (cs->target == atoms[XmA_MOTIF_EXPORT_TARGETS]")
end = text.index("\n  _XmConvertComplete(w, value, size, format, type, cs);", start)
block = text[start:end]

assert "Atom *targs = (Atom *) XtMalloc(sizeof(Atom) * 5);" in block
assert "if (targs == NULL)" in block
assert "_XmConvertComplete(w, NULL, 0, 8, XA_INTEGER, cs);" in block
assert "return;" in block

print("text selection target-list allocation guard: ok")
