#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"SelectioB.c").read_text()
assert "XmSelectionBoxGetChild(" in text
assert "if (sb == NULL)" in text
assert "return NULL;" in text
print("XmSelectionBox child lookup guards null widgets")
