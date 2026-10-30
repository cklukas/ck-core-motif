#!/usr/bin/env python3
"""Check Skeleton color resources request redisplay."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Skeleton.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean SetValues(Widget old_widget")
end = source.index("static XtGeometryResult QueryGeometry", start)
body = source[start:end]
for fragment in (
    "Boolean colors_changed;",
    "old->primitive.bottom_shadow_color != skeleton->primitive.bottom_shadow_color",
    "old->primitive.top_shadow_color != skeleton->primitive.top_shadow_color",
    "old->core.background_pixel != skeleton->core.background_pixel",
    "return changed || colors_changed ||",
):
    if fragment not in body:
        raise SystemExit(f"Skeleton color update contract missing: {fragment}")

print("Skeleton color changes request redisplay")
