#!/usr/bin/env python3
"""Check TabStack remains safe when its internal TabBox cannot be created."""

from pathlib import Path


source = Path("lib/Xm/TabStack.c").read_text()

initialize = source[source.index("Initialize(Widget request"):source.index("SetValues(Widget current")]
assert "if (XmTabStack_tab_box(ts) != NULL)" in initialize
assert initialize.index("if (XmTabStack_tab_box(ts) != NULL)") < initialize.index("XtAddCallback(XmTabStack_tab_box(ts)")

set_values = source[source.index("SetValues(Widget current"):source.index("QueryGeometry(Widget widget")]
assert "need_redraw && XmTabStack_tab_box(s_tab) != NULL" in set_values

query_start = source.index("QueryGeometry(Widget widget")
query = source[query_start:source.index("ConstraintInitialize", query_start)]
assert "if (XmTabStack_tab_box(tab) == NULL)" in query

layout = source[source.index("Layout(XmTabStackWidget tab)"):source.index("#endif /* TEAR_OFF_TABS */", source.index("Layout(XmTabStackWidget tab)"))]
assert "if (XmTabStack_tab_box(tab) == NULL)" in layout

geometry = source[source.index("GeometryManager(Widget widget"):source.index("ConstraintSetValues(Widget current")]
assert "if (XmTabStack_tab_box(tab) != NULL)" in geometry

print("TabStack TabBox allocation failures are null-safe")
