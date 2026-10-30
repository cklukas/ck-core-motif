#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"SelectioB.c").read_text()
assert "XmSelectionBoxGetChild(" in text
assert "if (sb == NULL || !XmIsSelectionBox(sb))" in text
assert "return NULL;" in text
print("XmSelectionBox child lookup guards invalid widgets")
