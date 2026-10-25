#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"XmExtUtil.c").read_text()
assert text.count("if (w == NULL)") >= 4
assert "return (False);" in text
assert "_XmSetValuesOnChildren" in text
print("Xm widget utility helpers guard null widgets")
