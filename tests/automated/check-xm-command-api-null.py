#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"Command.c").read_text()
assert text.count("widget == NULL") >= 4
assert "if (widget == NULL || value == NULL)" in text
assert "if (widget == NULL || error == NULL)" in text
for name in ("XmCommandGetChild", "XmCommandSetValue", "XmCommandAppendValue", "XmCommandError"):
    assert name in text
print("XmCommand APIs validate null widgets and strings")
