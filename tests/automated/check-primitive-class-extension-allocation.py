#!/usr/bin/env python3
"""Check that Primitive class-extension allocation is checked."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Primitive.c").read_text()
start = source.index("ClassPartInitialize(\n\t WidgetClass")
end = source.index("\n/************************************************************************", start)
region = source[start:end]

assert "*wcePtr = (XmPrimitiveClassExt) XtCalloc" in region
assert "if (*wcePtr == NULL)" in region
assert region.index("if (*wcePtr == NULL)") < region.index("(*wcePtr)->next_extension")
assert "return;" in region

print("PASS Primitive class-extension allocation failure is handled")
