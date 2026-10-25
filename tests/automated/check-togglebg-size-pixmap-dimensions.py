#!/usr/bin/env python3
"""Check ToggleButtonGadget sizing does not reuse stale pixmap dimensions."""
import re
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "ToggleBG.c"
text = source.read_text(encoding="utf-8")
start = text.index("SetToggleSize(\n")
end = text.index("/* END OSF Fix pir 1778 */", start)
body = text[start:end]
lookup = "XmeGetPixmapData(XtScreen(newtbg)"
lookups = body.count(lookup)
if lookups != 6:
    raise SystemExit(f"expected six ToggleButtonGadget lookups, found {lookups}")
guarded = re.findall(r"tmpW = tmpH = 0;\s*XmeGetPixmapData\(XtScreen\(newtbg\)", body)
if len(guarded) != lookups:
    raise SystemExit("a ToggleButtonGadget lookup is not preceded by a dimension reset")
print("ToggleButtonGadget pixmap dimensions are reset before every lookup")
