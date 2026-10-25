#!/usr/bin/env python3
"""Check tab-list copy validates signed offsets before subtraction."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "XmTabList.c"
text = source.read_text()
start = text.index("XmTabList\nXmTabListCopy")
end = text.index("/* \n * Marked tabs", start)
block = text[start:end]

if "offset_magnitude = TabPositionMagnitude(offset);" not in block:
    raise SystemExit("copy does not calculate a checked offset magnitude")
if "offset_magnitude >= _XmTabLCount(tablist)" not in block:
    raise SystemExit("copy does not reject out-of-range offsets")
if "_XmTabLCount(tablist) - offset_magnitude" not in block:
    raise SystemExit("copy still derives its default count from abs(offset)")

helper = text[text.index("TabPositionMagnitude"):text.index("/* \n * Marked tabs")]
if "-(position + 1)" not in helper:
    raise SystemExit("offset magnitude is not safe for INT_MIN")
if "abs(position) >= _XmTabLCount(tablist)" in text:
    raise SystemExit("tab lookup still applies abs to its unsigned position")

print("tab-list copy validates offsets without unsigned underflow")
