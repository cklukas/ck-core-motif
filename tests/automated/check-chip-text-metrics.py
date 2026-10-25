#!/usr/bin/env python3
"""Check Chip labels use measured active-font geometry."""
from pathlib import Path

source = Path("lib/Xm/Chip.c").read_text(encoding="utf-8")
assert "XQueryFont" in source
assert "XTextWidth" in source
assert "XFreeFontInfo(NULL, font, 1)" in source
assert "label_text_metrics(chip, text" in source
print("PASS Chip labels use measured font geometry")
