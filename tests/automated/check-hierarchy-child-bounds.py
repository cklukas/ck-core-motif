#!/usr/bin/env python3
"""Check hierarchy child-list export guards count conversion and allocation."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "Hierarchy.c"
text = source.read_text(encoding="utf-8")
start = text.index("if (child_count == (Cardinal)~0")
end = text.index("\n    for (i=0; i < child_count; i++)", start)
region = text[start:end]
assert "child_count == (Cardinal)~0" in region
assert "(size_t)child_count >" in region
assert "(size_t)((Cardinal)~0) / sizeof(Widget) - 1" in region
assert "(size_t)child_count + 1" in region
assert "XtMalloc((Cardinal)(((size_t)child_count + 1) *" in region
assert "if (retval == NULL)" in region
assert "return NULL;" in region
assert "Cardinal i;" in text
print("hierarchy child-list export bounds count and allocation")
