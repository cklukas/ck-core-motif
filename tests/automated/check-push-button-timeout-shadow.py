#!/usr/bin/env python3
"""Check PushButton timeout shadow redraw guards."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "PushB.c"
text = source.read_text(encoding="utf-8")
start = text.rindex("static void \nArmTimeout(")
end = text.find("\f", start)
body = text[start:] if end < 0 else text[start:end]
required = [
    "(int)pb->core.width > 2 * (int)pb->primitive.highlight_thickness",
    "(int)pb->core.height > 2 * (int)pb->primitive.highlight_thickness",
    "pb->primitive.top_shadow_GC != NULL",
    "pb->primitive.bottom_shadow_GC != NULL",
    "(int)pb->core.width - 2 * (int)pb->primitive.highlight_thickness",
    "(int)pb->core.height - 2 * (int)pb->primitive.highlight_thickness",
]
missing = [fragment for fragment in required if fragment not in body]
if missing:
    raise SystemExit("ArmTimeout shadow guard missing: " + ", ".join(missing))
print("PushButton timeout shadow guards are present")
