#!/usr/bin/env python3
"""Check that FontSelector encoding-list duplication is atomic on failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text(encoding="utf-8")
start = source.index("newList = (String*)XtMalloc(sizeof(String) * i);")
end = source.index("_XmFilterArgs", start)
region = source[start:end]
assert "if (newList == NULL)" in region
assert "newList[i] = XtNewString(*encodings);" in region
assert "if (newList[i] == NULL)" in region
assert "XtFree((char *)newList);" in region
assert "newList[i] = NULL;" in region
print("PASS FontSelector encoding-list duplication is guarded")
