#!/usr/bin/env python3
"""Check correctly sized, checked WM_COLORMAP_WINDOWS allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmProperty.c").read_text(
    encoding="utf-8"
)
start = source.index("void ProcessWmColormapWindows")
section = source[start:]
assert "size_t windowCount;" in section
assert "windowCount = (size_t)nitems + 1;" in section
assert "windowCount > UINT_MAX / sizeof(Window)" in section
assert "windowCount * sizeof(Window)" in section
assert "windowCount * sizeof(Colormap)" in section
assert "nitems * sizeof (Window)" not in section
assert "nitems*sizeof(Colormap)" not in section
print("WM_COLORMAP_WINDOWS arrays use checked element-sized allocations")
