#!/usr/bin/env python3
"""Check InfoBar close icon geometry is centered and symmetric."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "InfoBar.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Redisplay(")
end = source.index("\nstatic Boolean SetValues", start)
region = source[start:end]

for needle in (
    "int button_cx = button_x + 9;",
    "int button_cy = button_y + 9;",
    "button_cx - 4, button_cy - 4,",
    "button_cx + 4, button_cy + 4);",
    "button_cx + 4, button_cy - 4,",
    "button_cx - 4, button_cy + 4);",
):
    if needle not in region:
        raise SystemExit(f"InfoBar close icon geometry missing: {needle}")

print("PASS InfoBar close icon is centered and symmetric")
