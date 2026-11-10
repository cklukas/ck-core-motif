#!/usr/bin/env python3
"""Check WSM display and screen metadata reject invalid Xlib inputs safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "disp.c").read_text(
    encoding="utf-8"
)
start = source.index("_WSMGetDispInfo(Display *dpy)")
end = source.index("_WSMGetScreenInfo(Display *dpy", start)
function = source[start:end]

assert "if (dpy == NULL)" in function
assert function.index("if (dpy == NULL)") < function.index("XInternAtoms(dpy")

start = source.index("_WSMGetScreenInfo(Display *dpy")
end = source.index("_WSMClearConfigScreenInfo", start)
function = source[start:end]

guard = "screen_number < 0 || screen_number >= ScreenCount(dpy)"
assert guard in function
assert function.index(guard) < function.index("_WSMGetDispInfo(dpy)")

start = source.index("_WSMClearConfigScreenInfo(Display *dpy")
end = source.index("_WSMGetConfigFormat", start)
function = source[start:end]

assert guard in function
assert function.index(guard) < function.index("_WSMGetDispInfo(dpy)")

print("PASS WSM display and screen metadata reject invalid inputs")
