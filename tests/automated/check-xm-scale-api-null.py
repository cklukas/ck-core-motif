#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"Scale.c").read_text()
assert "if (w == NULL || !XmIsScale(w))" in text
assert "if (w == NULL || !XmIsScale(w) || value == NULL)" in text
print("XmScale APIs guard null widgets and output pointers")
