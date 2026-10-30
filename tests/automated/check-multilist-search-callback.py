#!/usr/bin/env python3
"""Check MultiList search activation validates callback and child state."""

from pathlib import Path


source = Path("lib/Xm/MultiList.c").read_text()
start = source.index("ActivateTextSearch(Widget w")
end = source.index("/************************************************************", start + 1)
body = source[start:end]

assert "w == NULL || elist_ptr == NULL || client == NULL" in body
assert "XmMultiList_find(elist) == NULL" in body
assert "XmMultiList_find_text(elist) == NULL" in body
assert "XmMultiList_ilist(elist) == NULL" in body
assert body.index("XmMultiList_ilist(elist) == NULL") < body.index("XmTextFieldGetString")
assert "find == NULL || find_text == NULL" not in body

print("MultiList search callback validates optional children and payload")
