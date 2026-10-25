#!/usr/bin/env python3
"""Check callback-list SIZE_MAX guards include their standard definition."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CallbackI.c").read_text()
assert "#include <stdint.h>" in source
assert "SIZE_MAX" in source
print("PASS callback SIZE_MAX header")
