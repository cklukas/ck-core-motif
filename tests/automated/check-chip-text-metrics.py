#!/usr/bin/env python3
"""Check Chip labels use measured active-font geometry."""
from pathlib import Path

source = Path("lib/Xm/Chip.c").read_text(encoding="utf-8")
assert "XmStringExtent(chip->chip.font, chip->chip.label" in source
assert "if (chip->chip.label && chip->chip.font)" in source
assert "XmStringDraw(display, window, chip->chip.font" in source
print("PASS Chip labels use render-table geometry")
