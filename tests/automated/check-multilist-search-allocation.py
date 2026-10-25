#!/usr/bin/env python3
"""Check MultiList search handles text extraction allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "MultiList.c").read_text()
start = source.index("ActivateTextSearch(")
end = source.index("/************************************************************\n*\tLOCAL CODE", start)
region = source[start:end]

assert "if (ptr == NULL)" in region
guard = region.index("if (ptr == NULL)")
assert region.index("XtFree((XtPointer)wc_string);", guard) < region.index("return;", guard)
assert guard < region.index("streq(ptr, XmMultiList_last_search(elist))")
assert guard < region.index("XmI18ListFindRow")
assert region.index("XmMultiList_last_search(elist) = ptr;") > region.index("XmI18ListFindRow")
print("MultiList search preserves state when text extraction fails")
