#!/usr/bin/env python3
"""Check that accessibility text snapshots handle result allocation failure."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "AccessibleDBus.c"
text = source.read_text(encoding="utf-8")
start = text.index("static char *text_snapshot(")
end = text.index("\nstatic ", start + 1)
region = text[start:end]
assert "result = XtMalloc((Cardinal)capacity);" in region
assert "if (!result)" in region
assert "return NULL;" in region
print("accessibility text snapshots guard result allocation")
