#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/List.c").read_text()
start = source.index("static int\nAddInternalElements(XmListWidget lw,")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "SIZE_MAX / sizeof(ElementPtr)" in body
assert "UINT_MAX / sizeof(ElementPtr)" in body
assert "(Cardinal)(sizeof(ElementPtr) * (size_t)lw->list.itemCount)" in body
assert "ElementPtr *new_internal" in body
assert "if (new_internal == NULL)" in body
assert "lw->list.InternalList = new_internal;" in body
assert "XtRealloc((char *)lw->list.InternalList" not in body
print("XmList internal-list growth guards present")
