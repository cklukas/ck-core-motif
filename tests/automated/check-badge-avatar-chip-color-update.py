#!/usr/bin/env python3
"""Check cached drawing GCs follow Badge, Avatar, and Chip color changes."""
from pathlib import Path

root = Path(__file__).parents[2]
contracts = {
    "Badge.c": ("badge->badge.foreground_gc", "badge->primitive.foreground"),
    "Avatar.c": ("avatar->avatar.foreground_gc", "avatar->primitive.foreground"),
    "Chip.c": ("chip->chip.foreground_gc", "chip->primitive.foreground"),
}
for name, (gc, foreground) in contracts.items():
    source = (root / "lib" / "Xm" / name).read_text(encoding="utf-8")
    start = source.index("static Boolean SetValues(Widget old_widget")
    end = source.index("static XtGeometryResult QueryGeometry", start)
    body = source[start:end]
    if name == "Avatar.c":
        assert "old->primitive.foreground != avatar->primitive.foreground" in body
        assert "old->core.background_pixel != avatar->core.background_pixel" in body
        draw = source[source.index("static void Redisplay(Widget widget"):start]
        assert "XSetForeground(display, gc, foreground);" in draw
        assert "XSetBackground(display, gc, avatar->core.background_pixel);" in draw
        continue
    for fragment in (
        "Boolean colors_changed =",
        f"if (colors_changed",
        f"XSetForeground(XtDisplay(widget), {gc},",
        f"{foreground}",
        "return",
    ):
        if fragment not in body:
            raise SystemExit(f"{name} color update contract missing: {fragment}")

print("Badge, Avatar, and Chip color changes refresh cached drawing GCs")
