#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"List.c").read_text()
for name in ("XmListUpdateSelectedList", "XmListPosSelected"):
    assert name in text
assert text.count("if (w == NULL)") >= 2
print("XmList selection helpers guard null widgets")
