#!/usr/bin/env python3
"""Check that DrawnButton menu lifecycle paths guard undersized geometry."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "DrawnB.c"
text = source.read_text(encoding="utf-8")

required = [
    "db->primitive.top_shadow_GC != NULL",
    "db->primitive.bottom_shadow_GC != NULL",
    "(int)db->core.width - 2 * (int)db->primitive.highlight_thickness",
    "(int)db->core.height - 2 * (int)db->primitive.highlight_thickness",
]
missing = [fragment for fragment in required if fragment not in text]
if missing:
    raise SystemExit("DrawnButton menu guard missing: " + ", ".join(missing))

for name in ("Leave", "BorderUnhighlight"):
    start = text.index("static void \n" + name)
    end = text.index("\f", start)
    body = text[start:end]
    if "XmeClearBorder" in body:
        if "(int)db->core.width > 2 * (int)db->primitive.highlight_thickness" not in body:
            raise SystemExit(f"{name} does not guard the clear-border width")
        if "(int)db->core.height > 2 * (int)db->primitive.highlight_thickness" not in body:
            raise SystemExit(f"{name} does not guard the clear-border height")

guard_start = text.index("if ((int)db->core.width > 2 * (int)db->primitive.highlight_thickness")
shadow_call = text.index("XmeDrawShadows", guard_start)
guard = text[guard_start:shadow_call]
if "db->primitive.top_shadow_GC != NULL" not in guard or "db->primitive.bottom_shadow_GC != NULL" not in guard:
    raise SystemExit("menu shadow path does not guard shadow GCs")

print("DrawnButton menu geometry guards are present")
