#!/usr/bin/env python3
"""Check Flyout popup creation and positioning failure handling."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/Flyout.c"
text = source.read_text(encoding="utf-8")

start = text.index("static void position_popup")
position = text[start:text.index("static void create_popup", start)]
assert "if (max_x < 0) x = 0;" in position
assert "if (max_y < 0) y = 0;" in position
start = text.index("static void create_popup")
body = text[start:text.index("static void Initialize", start)]
assert "if (!flyout->flyout.popup_shell) return;" in body
assert "if (!label_string)" in body
assert "if (!flyout->flyout.label)" in body
assert "XtDestroyWidget(flyout->flyout.popup_shell);" in body
print("Flyout popup creation and positioning are guarded")
