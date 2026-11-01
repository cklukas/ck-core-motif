#!/usr/bin/env python3
"""Check DataField highlight growth keeps state valid on allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(encoding="utf-8")
start = source.index("static void \n#ifdef _NO_PROTO\ndf_InsertHighlight(")
end = source.index("static void \n#ifdef _NO_PROTO\nDataFieldSetHighlight(", start)
region = source[start:end]

assert "_XmHighlightRec *new_list;" in region
assert "Cardinal old_maximum;" in region
assert "size_t allocation_size;" in region
assert "if (allocation_size > (size_t)UINT_MAX)" in region
assert "new_list = (_XmHighlightRec *)XtRealloc((char *)l," in region
assert "if (new_list == NULL)" in region
assert "XmTextF_highlight(w).number--;" in region
assert "XmTextF_highlight(w).maximum = old_maximum;" in region
assert "l = XmTextF_highlight(w).list = new_list;" in region
assert "XmTextF_highlight(w).list = (_XmHighlightRec *)XtRealloc" not in region
print("PASS DataField highlight growth is allocation-safe")
