#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"Paned.c").read_text()
assert "if (w == NULL || panes == NULL || num == NULL)" in text
assert "XmPanedGetPanes" in text
print("XmPanedGetPanes validates widget and outputs")
