#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"ChColor.c").read_text()
assert "if (w == NULL)" in text
assert "XmChangeColor(" in text
print("XmChangeColor guards null widgets")
