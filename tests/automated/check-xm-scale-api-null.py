#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"Scale.c").read_text()
assert text.count("if (w == NULL") >= 2
assert "if (w == NULL || value == NULL)" in text
print("XmScale APIs guard null widgets and output pointers")
