#!/usr/bin/env python3
"""Check ProgressRing initialization has an XmString copy-failure fallback."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/ProgressRing.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Initialize(Widget request")
end = source.index("static void Destroy", start)
context = source[start:end]
required = (
    "XmString label_copy;",
    "label_copy = ring->progress_ring.label",
    "ring->progress_ring.label = label_copy",
    "? label_copy : XmStringCreateLocalized(\"\");",
)
missing = [fragment for fragment in required if fragment not in context]
if missing:
    raise SystemExit("ProgressRing label initialization fallback missing: " + ", ".join(missing))

print("ProgressRing label-init check passed")
