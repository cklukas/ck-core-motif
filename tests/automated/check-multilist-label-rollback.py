#!/usr/bin/env python3
"""Check MultiList label replacements are transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "MultiList.c").read_text()
start = source.index("static Boolean \nSetValues(Widget current")
end = source.index("/*\tFunction Name: GeometryManager", start)
region = source[start:end]

assert "XmString replacement;" in region
assert "replacement = XmStringCopy(XmMultiList_find_label(e_set));" in region
assert "XmMultiList_find_label(e_set) = XmMultiList_find_label(e_old);" in region
assert "replacement = XmStringCopy(*title);" in region
assert "*title = old_title;" in region
assert "if (*title != old_title)" in region
assert region.index("XmStringCopy(XmMultiList_find_label(e_set))") < region.index("XmStringFree(XmMultiList_find_label(e_old))")
assert region.index("XmStringCopy(*title)") < region.index("XmStringFree(old_title)")
print("MultiList labels preserve old values on copy failure")
