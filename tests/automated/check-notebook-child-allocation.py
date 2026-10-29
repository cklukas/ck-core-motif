#!/usr/bin/env python3
"""Check Notebook scroller construction and use tolerate allocation failure."""

from pathlib import Path


source = Path("lib/Xm/Notebook.c").read_text()

create = source[source.index("CreateTabScrollers ("):source.index("SetVisualConfig", source.index("CreateTabScrollers ("))]
assert create.count("XmeTraitGet((XtPointer)") >= 8
assert create.count("if (nb->notebook.") >= 4
assert "XmQTactivatable) != NULL" in create

page_start = source.index("SetPageScroller (")
page = source[page_start:source.index("LayoutChildren (", page_start)]
assert "if (nb->notebook.scroller == NULL)" in page
assert page.index("if (nb->notebook.scroller == NULL)") < page.index("XtVaCreateManagedWidget(NB_TEXT_FIELD_NAME")
assert "nb->notebook.scroller_child == NULL" in page
assert "XtDestroyWidget(nb->notebook.scroller)" in page
assert "nb->notebook.scroller_status = DEFAULT_GONE" in page

redisplay_start = source.index("Redisplay (")
redisplay = source[redisplay_start:source.index("DrawBackPages", redisplay_start)]
assert "if (nb->notebook.prev_major != NULL)" in redisplay
assert "if (nb->notebook.prev_minor != NULL)" in redisplay

major = source[source.index("LayoutMajorTabs ("):source.index("LayoutMinorTabs (", source.index("LayoutMajorTabs ("))]
minor_start = source.index("LayoutMinorTabs (")
minor = source[minor_start:source.index("ResetTopPointers", minor_start)]
assert "nb->notebook.prev_major != NULL && nb->notebook.next_major != NULL" in major
assert "nb->notebook.prev_minor != NULL && nb->notebook.next_minor != NULL" in minor

assert "nb->notebook.scroller_child != NULL" in source[source.index("UpdateNavigators ("):]

print("Notebook scroller allocation failures are null-safe")
