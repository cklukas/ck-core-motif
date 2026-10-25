#!/usr/bin/env python3
"""Check ToggleButton sizing does not reuse stale pixmap dimensions."""
import re
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "ToggleB.c"
text = source.read_text(encoding="utf-8")
start = text.index("SetToggleSize(\n")
end = text.index("/* END OSF Fix pir 1778 */", start)
body = text[start:end]

lookups = body.count("XmeGetPixmapData(XtScreen(newtb)")
if lookups != 6:
    raise SystemExit(f"expected six ToggleButton pixmap size lookups, found {lookups}")
guarded = re.findall(
    r"tmpW = tmpH = 0;\s*XmeGetPixmapData\(XtScreen\(newtb\)", body
)
if len(guarded) != lookups:
    raise SystemExit("a pixmap lookup is not preceded by a dimension reset")
print("ToggleButton pixmap dimensions are reset before every lookup")
