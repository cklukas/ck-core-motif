#!/usr/bin/env python3
"""Check FontS size and alternate-family list allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text()
assert "strs == NULL)\n\treturn;" in source
assert "strs == NULL && num != 0" in source
