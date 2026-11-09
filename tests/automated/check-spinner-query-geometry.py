#!/usr/bin/env python3
"""Check Spinner retains explicit geometry and restores per-axis auto sizing."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Spinner.c"
text = source.read_text(encoding="utf-8")
set_start = text.index("static Boolean SetValues(Widget old_widget")
set_end = text.index("\nstatic void PreferredSize", set_start)
set_values = text[set_start:set_end]

assert "Boolean geometry_changed = False;" in set_values
assert "Boolean automatic = (Dimension)args[i].value == 0;" in set_values
assert "spinner->spinner.width_default = automatic;" in set_values
assert "spinner->spinner.height_default = automatic;" in set_values
assert "if (redraw || geometry_changed)" in set_values
assert "return redraw || geometry_changed;" in set_values

query_start = text.index("static XtGeometryResult QueryGeometry(Widget widget")
query_end = text.index("\n\n", query_start)
query = text[query_start:query_end]
assert "XmSpinnerWidget spinner = (XmSpinnerWidget)widget;" in query
assert "if (!spinner->spinner.width_default)" in query
assert "reply->width = spinner->core.width;" in query
assert "if (!spinner->spinner.height_default)" in query
assert "reply->height = spinner->core.height;" in query

print("Spinner QueryGeometry preserves explicit dimensions and auto axes")
