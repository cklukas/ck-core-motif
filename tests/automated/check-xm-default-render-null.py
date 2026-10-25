#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"ResConvert.c").read_text()
assert "XmeGetDefaultRenderTable(" in text
assert "if (w == NULL)" in text
assert "return NULL;" in text
print("Xm default render lookup guards null widgets")
