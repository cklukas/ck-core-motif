#!/usr/bin/env python3
"""Check drag-context creation fails before callbacks or startup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragC.c").read_text(
    encoding="utf-8"
)
start = source.index('XtCreateWidget("dragContext"')
end = source.index("_XmDragStart", start)
region = source[start:end]
assert "if (dc == NULL)" in region
assert "return NULL;" in region
assert region.index("if (dc == NULL)") < region.index("XtAddCallback")
print("PASS drag-context construction handles allocation failure")
