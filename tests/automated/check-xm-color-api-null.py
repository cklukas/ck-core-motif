#!/usr/bin/env python3
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "Color.c").read_text()
assert "if (screen == NULL)" in text
assert "if (widget == NULL || value == NULL)" in text
assert "if (foreground_ret)" in text
print("Xm color APIs validate screen, widget, and output inputs")
