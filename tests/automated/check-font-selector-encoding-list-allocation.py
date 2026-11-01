#!/usr/bin/env python3
"""Check that FontSelector encoding-list duplication is atomic on failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text(encoding="utf-8")
start = source.index("newList = (String *)XtCalloc(count + 1, (Cardinal)sizeof(*newList));")
end = source.index("_XmFilterArgs", start)
region = source[start:end]
assert "if (newList == NULL)" in region
assert "newList[count] = XtNewString(encodings[count]);" in region
assert "if (newList[count] == NULL)" in region
assert "XtFree((char *)newList);" in region
assert "XtFree(newList[--count]);" in region
assert region.index("if (newList[count] == NULL)") < region.index("ENCODING_LIST(fsw) = newList;")
print("PASS FontSelector encoding-list duplication is guarded")
