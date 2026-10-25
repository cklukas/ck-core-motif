#!/usr/bin/env python3
"""Check XmRenderTableGetTags validates and rolls back its result array."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("XmRenderTableGetTags(")
end = source.index("/* Returns copy of matching rendition. */", start)
region = source[start:end]

assert "if (tag_list == NULL)" in region
assert "*tag_list = NULL;" in region
assert "SIZE_MAX / sizeof(*tags)" in region
assert "tags = (XmStringTag *)XtMalloc" in region
assert "if (tags == NULL)" in region
assert "XtNewString(_XmRendTag" in region
assert "while (i != 0)" in region
assert "XtFree((char *)tags);" in region
print("PASS render-table tag result handles allocation failure")
