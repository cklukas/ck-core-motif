#!/usr/bin/env python3
"""Check mutable XmString concatenation handles entry-array resize failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmStringConcatAndFree(")
end = source.index("\n\n/*", start)
region = source[start:end]

assert "_XmStringEntry *new_entries;" in region
assert "new_entries = (_XmStringEntry *)XtRealloc" in region
assert "if (new_entries == NULL)" in region
assert "_XmStrEntry(a_str) = new_entries;" in region
print("PASS mutable XmString concatenation rolls back entry resize failure")
