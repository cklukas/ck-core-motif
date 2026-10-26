#!/usr/bin/env python3
"""Check bounded XmNumberBox text lengths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "NumberBox.c").read_text(encoding="utf-8")
assert "char text[64];" in (Path(__file__).parents[2] / "lib" / "Xm" / "NumberBoxP.h").read_text(encoding="utf-8")
assert "sizeof(number->number_box.text)" in source
assert "length_now + 1 < sizeof(number->number_box.text)" in source
print("PASS number-box text lengths are bounded")
