#!/usr/bin/env python3
"""Ensure ScrollBar dispatch leaves compiled callback-list ownership to Xt."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrollBar.c").read_text()
start = source.index("static void\nScrollCallback(")
body = source[start:source.index("\n}\n", start)]
assert "XtCallCallbackList" in body
assert "CallScrollCallbackList" not in source
assert "callbacks[count].callback" not in source
print("PASS ScrollBar uses Xt-owned compiled callback dispatch")
