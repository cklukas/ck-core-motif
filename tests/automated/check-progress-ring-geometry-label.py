#!/usr/bin/env python3
"""Check ProgressRing geometry measures only a non-null label."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/ProgressRing.c").read_text(
    encoding="utf-8"
)
start = source.index("static XtGeometryResult QueryGeometry")
end = source.index("static void Notify", start)
context = source[start:end]
required = "if (ring->progress_ring.font && ring->progress_ring.label)"
if required not in context:
    raise SystemExit("ProgressRing geometry must guard a null label")
redisplay_start = source.index("static void Redisplay(Widget widget")
redisplay_end = source.index("static Boolean SetValues", redisplay_start)
redisplay = source[redisplay_start:redisplay_end]
if required not in redisplay:
    raise SystemExit("ProgressRing redisplay must guard a null label")

print("ProgressRing geometry-label check passed")
