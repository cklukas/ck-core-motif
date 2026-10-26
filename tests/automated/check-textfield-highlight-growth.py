#!/usr/bin/env python3
"""Check TextField highlight-list growth handles realloc failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void \nInsertHighlight")
end = source.index("\nstatic void \nTextFieldSetHighlight", start)
body = source[start:end]

assert "allocation_size > (size_t)UINT_MAX" in body
assert "new_list = (_XmHighlightRec *)XtRealloc" in body
assert "if (new_list == NULL)" in body
assert "w->text.highlight.number--;" in body
assert "w->text.highlight.maximum = old_maximum;" in body

print("TextField highlight growth preserves state on allocation failure")
