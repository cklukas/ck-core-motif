#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"XmExtUtil.c").read_text()
needle="if (w == NULL || (args == NULL && num_args != 0))"
assert needle in text
print("Xm child updates validate null argument lists")
