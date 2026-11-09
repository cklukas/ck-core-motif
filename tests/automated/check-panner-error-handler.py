#!/usr/bin/env python3
"""Check Panner uses the Xlib error-handler contract during redraw."""

from pathlib import Path


source = (
    Path(__file__).resolve().parents[2]
    / "demos"
    / "programs"
    / "panner"
    / "panner.c"
).read_text(encoding="utf-8")

assert "XErrorHandler oldHandler;" in source
assert "oldHandler = XSetErrorHandler(IgnoreError);" in source
assert "XSetErrorHandler(oldHandler);" in source
assert (
    "if (XGetWindowAttributes(pInfoList[DSP].display, child[i], &attr) != 0 &&"
    in source
)
assert "int (*oldHandler)();" not in source

print("Panner uses the typed Xlib error-handler and guarded attributes")
