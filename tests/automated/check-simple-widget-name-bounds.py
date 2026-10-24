#!/usr/bin/env python3
"""Check that Simple widget names are formatted into sufficiently bounded storage."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Simple.c").read_text()
assert source.count("char name_buf[32];") == 3
assert source.count("snprintf(name_buf, sizeof(name_buf),") == 7
assert "sprintf(name_buf" not in source
print("Simple widget names use bounded formatting")
