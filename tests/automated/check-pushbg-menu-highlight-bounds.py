#!/usr/bin/env python3
"""Check PushBG menu highlighting passes signed inset dimensions."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PushBG.c").read_text()

assert "(int) pb->rectangle.width -" in source
assert "(int) pb->rectangle.height -" in source

print("PushBG menu highlight dimensions are signed")
