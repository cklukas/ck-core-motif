#!/usr/bin/env python3
"""Check client-list growth is bounded before realloc and Xlib conversion."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients/mwm/WmWinList.c").read_text(
    encoding="utf-8"
)
start = source.index("void UpdateScreenClientList")
end = source.index("}  /* END OF FUNCTION UpdateScreenClientList", start)
body = source[start:end]
assert "Window *windows = NULL" in body
assert "if (pSD == NULL)" in body
assert "pEntry->pCD != NULL" in body
assert "count == INT_MAX" in body
assert "(size_t)(count + 1) > (size_t)UINT_MAX / sizeof(Window)" in body
assert "(size_t)(count + 1) * sizeof (Window)" in body
assert "SetMwmClientList (pSD->rootWindow, windows, (unsigned int)count)" in body
assert "realloc (windows, (count + 1) * sizeof (Window))" not in body
print("PASS screen client-list growth and count conversion are bounded")
