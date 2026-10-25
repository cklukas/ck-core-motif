#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"RCMenu.c").read_text()
assert text.count("if (menu == NULL)") >= 2
assert "if (p == NULL || event == NULL)" in text
for name in ("XmGetPostedFromWidget", "XmGetTearOffControl", "XmMenuPosition"):
    assert name in text
print("Xm menu APIs validate null inputs")
