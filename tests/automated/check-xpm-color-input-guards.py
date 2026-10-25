#!/usr/bin/env python3
"""Check XPM color helpers reject absent screen, color, or output state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()

def body(name: str, next_name: str) -> str:
    start = source.rindex(name)
    start = source.index("\n{", start) + 2
    end = source.index(next_name, start)
    return source[start:end]

complete = body("CompleteUnspecColors(\n", "GetOverrideColors(")
assert "screen == NULL || override_colors == NULL" in complete

override = body("GetOverrideColors(\n", "#ifdef _ORIG_GET_ICON_FILE")
assert "screen == NULL || acc_color == NULL || override_colors == NULL" in override
assert "return 0;" in override
print("PASS XPM color helper input guards")
