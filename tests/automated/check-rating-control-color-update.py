#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/RatingControl.c").read_text()
s = "".join(source.split())
assert "XSetForeground(display,rating->rating.gc," in s
assert "rating->primitive.highlight_color" in s
body=s[s.index("staticBooleanSetValues(Widgetold_widget"):s.index("staticXtGeometryResultQueryGeometry(Widgetwidget")]
assert "returnTrue;" in body
print("PASS rating-control-color-update")
