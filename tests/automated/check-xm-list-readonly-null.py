#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"List.c").read_text()
assert text.count("if (w == NULL)") >= 2
assert text.count("w == NULL || item == NULL") >= 2
for name in ("XmListItemExists", "XmListItemPos", "XmListGetKbdItemPos", "XmListYToPos"):
    assert name in text
print("XmList read-only queries validate null widgets and items")
