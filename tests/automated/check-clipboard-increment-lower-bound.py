#!/usr/bin/env python3
"""Ensure clipboard INCR chunks remain positive on small X request limits."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("#define MAX_SELECTION_INCR")
end = source.index("#define BYTELENGTH", start)
macro = source[start:end]
assert "XMaxRequestSize(dpy) > 25" in macro
assert ") : 1)" in macro
print("PASS clipboard INCR size has a positive lower bound")
