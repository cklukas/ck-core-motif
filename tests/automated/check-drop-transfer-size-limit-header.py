#!/usr/bin/env python3
"""Check drop-transfer SIZE_MAX guards include their standard definition."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text()
assert "#include <stdint.h>" in source
assert "SIZE_MAX" in source
print("PASS drop-transfer SIZE_MAX header")
