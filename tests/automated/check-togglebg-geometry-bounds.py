#!/usr/bin/env python3
"""Check ToggleButtonGadget shadow drawing handles undersized gadgets safely."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "ToggleBG.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void \nDrawToggleShadow(")
end = text.index("/* BEGIN OSF Fix pir 1778 */", start)
body = text[start:end]
required = [
    "(int)tb->rectangle.width <= (hilite_thickness << 1)",
    "(int)tb->rectangle.height <= (hilite_thickness << 1)",
    "top_gc == NULL || bot_gc == NULL",
    "width = (int)tb->rectangle.width - (hilite_thickness << 1);",
    "height = (int)tb->rectangle.height - (hilite_thickness << 1);",
]
missing = [fragment for fragment in required if fragment not in body]
if missing:
    raise SystemExit("ToggleButtonGadget geometry guard missing: " + ", ".join(missing))
if "tb->rectangle.width - (hilite_thickness << 1)" in body and \
        "width = (int)tb->rectangle.width -" not in body:
    raise SystemExit("ToggleButtonGadget still subtracts unsigned dimensions")
print("ToggleButtonGadget shadow geometry guards are present")
