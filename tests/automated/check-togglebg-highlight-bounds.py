#!/usr/bin/env python3
"""Check ToggleBG menu highlight passes signed inset dimensions."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleBG.c").read_text()

assert "(int) w->rectangle.width -" in source
assert "2 * (int) w->gadget.highlight_thickness" in source
assert "w->rectangle.width - 2 *" not in source

print("ToggleBG highlight dimensions are signed")
