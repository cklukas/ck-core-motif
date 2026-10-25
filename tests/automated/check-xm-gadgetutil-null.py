#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"GadgetUtil.c").read_text()
assert "if (wid == NULL)" in text
assert "if (w == NULL || (region == NULL && event == NULL))" in text
print("Xm gadget utilities validate widget and event inputs")
