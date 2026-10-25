#!/usr/bin/env python3
"""Check ProgressBar label replacement is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ProgressBar.c").read_text()
start = source.index("static Boolean SetValues(Widget old_widget")
end = source.index("static XtGeometryResult QueryGeometry", start)
region = source[start:end]

assert "XmString replacement;" in region
assert "replacement = progress->progress.label != NULL" in region
assert "if (progress->progress.label == NULL || replacement != NULL)" in region
assert "progress->progress.label = old->progress.label;" in region
assert region.index("XmStringCopy(progress->progress.label)") < region.index("XmStringFree(old->progress.label)")
print("ProgressBar preserves labels on copy failure")
