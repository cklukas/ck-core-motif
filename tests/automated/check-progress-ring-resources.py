#!/usr/bin/env python3
"""Check ProgressRing preserves labels and falls back from a missing text GC."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/ProgressRing.c").read_text(
    encoding="utf-8"
)
redisplay_start = source.index("static void Redisplay(Widget widget")
setvalues_start = source.index("static Boolean SetValues(Widget old_widget", redisplay_start)
redisplay = source[redisplay_start:setvalues_start]
setvalues_end = source.index("static Dimension RingDiameter", setvalues_start)
setvalues = source[setvalues_start:setvalues_end]
required_redisplay = (
    "XtIsSensitive(widget) && ring->progress_ring.text_gc",
    ": ring->progress_ring.track_gc;",
)
missing = [fragment for fragment in required_redisplay if fragment not in redisplay]
if missing:
    raise SystemExit("ProgressRing text-GC fallback missing: " + ", ".join(missing))
required_setvalues = (
    "XmString replacement =",
    "if (!ring->progress_ring.label || replacement)",
    "ring->progress_ring.label = old->progress_ring.label;",
)
missing = [fragment for fragment in required_setvalues if fragment not in setvalues]
if missing:
    raise SystemExit("ProgressRing label rollback missing: " + ", ".join(missing))

print("ProgressRing resource checks passed")
