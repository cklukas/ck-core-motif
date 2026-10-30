#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"MessageB.c").read_text()
assert "XmMessageBoxGetChild(" in text
assert "if (widget == NULL || !XmIsMessageBox(widget))" in text
assert "return NULL;" in text
print("XmMessageBox child lookup guards invalid widgets")
