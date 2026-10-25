#!/usr/bin/env python3
"""Check that ScrollFrameT includes the SIZE_MAX definition."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ScrollFrameT.c"
text = source.read_text(encoding="utf-8")
assert "#include <stdint.h>" in text
assert "SIZE_MAX" in text
print("scroll-frame size-limit header present")
