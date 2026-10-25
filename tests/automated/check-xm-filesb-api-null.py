#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"FileSB.c").read_text()
assert text.count("if (fs == NULL)") >= 2
assert "XmFileSelectionBoxGetChild(" in text
assert "XmFileSelectionDoSearch(" in text
print("XmFileSelectionBox APIs guard null widgets")
