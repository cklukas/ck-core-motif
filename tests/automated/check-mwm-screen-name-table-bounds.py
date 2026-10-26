#!/usr/bin/env python3
"""Check MWM screen-name table sizing before Xt allocation."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmInitWs.c").read_text(
    encoding="utf-8"
)
start = source.index("void InitScreenNames (void)")
end = source.index("#ifndef NO_MESSAGE_CATALOG", start)
region = source[start:end]
assert "size_t screen_name_bytes;" in region
assert "numScreens < 0" in region
assert "(size_t)numScreens > SIZE_MAX / sizeof(char *)" in region
assert "(size_t)numScreens > (size_t)UINT_MAX / sizeof(char *)" in region
assert "screen_name_bytes = (size_t)numScreens * sizeof(char *);" in region
assert "XtMalloc((Cardinal)screen_name_bytes)" in region
assert "XtMalloc (numScreens * sizeof(char *))" not in region
print("PASS MWM screen-name table allocation is bounded")
