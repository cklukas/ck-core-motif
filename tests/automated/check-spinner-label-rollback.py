#!/usr/bin/env python3
"""Check Spinner label replacement is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Spinner.c").read_text()
start = source.index("static Boolean SetValues(Widget old_widget")
end = source.index("static XtGeometryResult QueryGeometry", start)
region = source[start:end]

assert "XmString replacement;" in region
assert "replacement = spinner->spinner.label != NULL" in region
assert "if (spinner->spinner.label == NULL || replacement != NULL)" in region
assert "spinner->spinner.label = old->spinner.label;" in region
assert region.index("XmStringCopy(spinner->spinner.label)") < region.index("XmStringFree(old->spinner.label)")
print("Spinner preserves labels on copy failure")
