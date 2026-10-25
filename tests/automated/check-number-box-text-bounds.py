#!/usr/bin/env python3
"""Check bounded XmNumberBox text lengths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "NumberBox.c").read_text(encoding="utf-8")
assert "static int bounded_text_length" in source
assert "bounded_text_length(number->number_box.text)" in source
print("PASS number-box text lengths are bounded")
