#!/usr/bin/env python3
"""Check FontSelector family/size refreshes reject partial XmString arrays."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text()

family_start = source.index("UpdateFamilies(XmFontSelectorWidget fsw)")
family_end = source.index("/*\tFunction Name:", family_start + 20)
family = source[family_start:family_end]
assert "strs[count - 1] == NULL" in family
assert "while (count != 0)" in family
assert "XmStringFree(strs[--count])" in family
assert "XtFree((XtPointer)strs)" in family

size_start = source.index("UpdateSizes(XmFontSelectorWidget fsw)")
size_end = source.index("/*\tFunction Name:", size_start + 20)
size = source[size_start:size_end]
assert "strs[count - 1] == NULL" in size
assert "strs[count] = XmStringCreateLocalized(buf)" in size
assert "if (strs[count] == NULL)" in size
assert "count++;" in size
print("FontSelector family and size refreshes reject partial string arrays")
