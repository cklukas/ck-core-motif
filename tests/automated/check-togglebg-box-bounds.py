#!/usr/bin/env python3
"""Check ToggleButtonGadget DrawBox rejects invalid geometry and GCs."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "ToggleBG.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void\nDrawBox(")
end = text.index("/*************************************<->*************************************", start)
body = text[start:end]
required = [
    "if (edge <= 0 || top_gc == NULL || bot_gc == NULL)",
    "XmeDrawShadows(XtDisplay ((Widget) w)",
    "if (edge > (shadow * 2))",
]
missing = [fragment for fragment in required if fragment not in body]
if missing:
    raise SystemExit("ToggleButtonGadget DrawBox guard missing: " + ", ".join(missing))
print("ToggleButtonGadget DrawBox geometry guard is present")
