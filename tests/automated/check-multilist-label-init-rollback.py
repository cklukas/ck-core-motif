#!/usr/bin/env python3
"""Check MultiList initialization handles label-copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "MultiList.c").read_text()
start = source.index("static void Initialize(Widget req")
end = source.index("\n    XmMultiList_title_wid(elist)", start)
region = source[start:end]

assert "XmString replacement;" in region
assert region.count("XmStringCopy(XmMultiList_") == 3
assert region.count("XmStringCreateLocalized(\"\")") >= 2
assert "XmStringGenerate(FIND_STRING" in region
print("MultiList initialization handles label allocation failure")
