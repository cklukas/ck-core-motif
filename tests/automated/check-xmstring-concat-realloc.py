#!/usr/bin/env python3
"""Check optimized XmString concatenation handles resize failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmStringConcatAndFree(")
end = source.index("\n\n/*", start)
region = source[start:end]

assert "XmString resized_b;" in region
assert "resized_b = (XmString)XtRealloc" in region
assert "if (resized_b == NULL)" in region
assert "_XmStrRefCountSet(b, 1);" in region
assert "memmove(_XmEntryTextGet" in region
print("PASS optimized XmString concatenation rolls back failed resize")
