#!/usr/bin/env python3
"""Check WSM display metadata rejects a missing X display safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "disp.c").read_text(
    encoding="utf-8"
)
start = source.index("_WSMGetDispInfo(Display *dpy)")
end = source.index("_WSMGetScreenInfo(Display *dpy", start)
function = source[start:end]

assert "if (dpy == NULL)" in function
assert function.index("if (dpy == NULL)") < function.index("XInternAtoms(dpy")

print("PASS WSM display metadata rejects null displays")
