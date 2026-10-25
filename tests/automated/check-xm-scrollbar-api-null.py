#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"ScrollBar.c").read_text()
assert text.count("if (w == NULL)") >= 2
assert "if (value) *value" in text
assert "if (slider_size) *slider_size" in text
print("XmScrollBar APIs guard null widgets and preserve optional outputs")
