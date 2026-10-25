#!/usr/bin/env python3
"""Check CascadeBG menu highlighting passes signed inset dimensions."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CascadeBG.c").read_text()

assert "(int) cb->rectangle.width -" in source
assert "(int) cb->rectangle.height -" in source

print("CascadeBG highlight dimensions are signed")
