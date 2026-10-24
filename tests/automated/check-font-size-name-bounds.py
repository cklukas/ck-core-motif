#!/usr/bin/env python3
"""Check that generated font-size labels use bounded numeric formatting."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text()
assert "char buf[32];" in source
assert 'snprintf(buf, sizeof(buf), "%d", GValidSizes[i]);' in source
assert 'sprintf(buf, "%d", GValidSizes[i]);' not in source
print("font selector size labels use bounded formatting")
