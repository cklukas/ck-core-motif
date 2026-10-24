#!/usr/bin/env python3
"""Check that ColorSelector RGB names use bounded formatting."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c").read_text()
start = source.index("static Boolean\nUpdateColorWindow")
end = source.index("\n}\n\n/*", start) + 2
region = source[start:end]
assert region.count("snprintf(buf, sizeof(buf), \"#%02x%02x%02x\"") == 2
assert 'sprintf(buf, "#%02x%02x%02x"' not in region
print("ColorSelector RGB formatting is bounded")
