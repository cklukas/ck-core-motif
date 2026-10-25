#!/usr/bin/env python3
"""Check compound-string segment copies roll back allocation failures."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "XmStringSeg.c"
text = source.read_text()
start = text.index("Boolean\n_XmStringGetSegment")
end = text.index("Boolean \n_XmStringGetNextSegment", start)
block = text[start:end]

if "if (copied_tag == NULL)" not in block:
    raise SystemExit("segment tag copy is not guarded")
if "if (tmp == NULL)" not in block:
    raise SystemExit("segment text copy is not guarded")
if "XtFree((char *)*tag);" not in block:
    raise SystemExit("partial segment copy is not released")

print("compound-string segment copies guard allocation failures")
