#!/usr/bin/env python3
"""Check widget geometry helpers validate widgets and output pointers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "WidGet.c").read_text(encoding="utf-8")
baseline = source[source.index("XmWidgetGetBaselines("):]
display = source[source.index("XmWidgetGetDisplayRect("):]
assert "if (wid == NULL || baselines == NULL || line_count == NULL)" in baseline
assert baseline.index("if (wid == NULL || baselines == NULL || line_count == NULL)") < baseline.index("_XmWidgetToAppContext(wid)")
assert "if (wid == NULL || displayrect == NULL)" in display
assert display.index("if (wid == NULL || displayrect == NULL)") < display.index("_XmWidgetToAppContext(wid)")
print("PASS widget geometry helpers validate input pointers")
