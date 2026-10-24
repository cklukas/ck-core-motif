#!/usr/bin/env python3
"""Check Mrm string-list append frees its allocation on pointer-list failure."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmptrlist.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrmPlistAppendString (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
append = region.index("result = UrmPlistAppendPointer")
free = region.index("XtFree (newstg)")
if free < append:
    raise SystemExit("Mrm string-list cleanup occurs before append result")
if "if (result != MrmSUCCESS)" not in region:
    raise SystemExit("Mrm string-list append does not condition cleanup on failure")
print("Mrm string-list append frees allocated strings after pointer-list failure")
