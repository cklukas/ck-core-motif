#!/usr/bin/env python3
"""Check XmScrolledWindowSetAreas rejects a NULL widget before XtSetValues."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrolledW.c").read_text(
    encoding="utf-8"
)
start = source.index("XmScrolledWindowSetAreas(")
end = source.index("\n}\n", start)
region = source[start:end]
assert "if (w == NULL)" in region
assert "return;" in region
assert region.index("if (w == NULL)") < region.index("XtSetValues(w, args, n)")
print("PASS XmScrolledWindowSetAreas validates its widget")
