#!/usr/bin/env python3
"""Ensure tiny List allocations keep horizontal-scrollbar decisions valid."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean\nSetHorizontalScrollbar(XmListWidget lw)")
body = source[start : source.index("\n/************************************************************************", start + 1)]

assert "was_managed = XtIsManaged((Widget) lw->list.hScrollBar);" in body
assert "is_managed = XtIsManaged((Widget) lw->list.hScrollBar);" in body
assert "available_width = (listwidth > 0) ? listwidth : 0;" in body
assert "(int)lw->list.MaxWidth <= available_width" in body
assert "(lw->list.MaxWidth <= (Dimension)listwidth)" not in body
print("PASS List horizontal scrollbar fit uses signed width and horizontal state")
