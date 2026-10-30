#!/usr/bin/env python3
"""Ensure Avatar SetValues validates old and new widget records."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/Avatar.c").read_text()
start = source.index("static Boolean SetValues(Widget old_widget")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "old_widget == NULL || widget == NULL" in body
assert body.count("XtIsSubclass") >= 2
assert body.index("if (old_widget == NULL") < body.index("font_changed =")
print("Avatar SetValues validates old and new widget records")
