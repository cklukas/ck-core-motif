#!/usr/bin/env python3
"""Check wide-text segment conversion guards its output allocation."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "XmStringSeg.c"
text = source.read_text()
start = text.index("Boolean \n_XmStringGetNextSegment")
end = text.index("XmStringGetNextSegment(XmStringContext", start)
block = text[start:end]

if "if (*text == NULL)" not in block:
    raise SystemExit("wide-text conversion does not guard allocation")
if "wcstombs(*text, wtext, len)" not in block:
    raise SystemExit("wide-text conversion call is missing")
if "XtFree((char *)*tag); *tag = NULL;" not in block:
    raise SystemExit("wide-text failure does not clear the tag")

print("wide-text segment conversion guards allocation failure")
