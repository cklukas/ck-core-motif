#!/usr/bin/env python3
"""Check that SCREEN_GAMMA cannot inject invalid values into libpng."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Png.c").read_text()
start = source.index("get_display_exponent(void)")
end = source.index("\n}\n\n/* return value", start)
region = source[start:end]

assert "errno = 0;" in region
assert "parsed = strtod(p, &end);" in region
assert "end != p && *end == '\\0'" in region
assert "errno != ERANGE" in region
assert "isfinite(parsed)" in region
assert "parsed > 0.0" in region
assert "display_exponent = default_display_exponent;" in region
assert "atof(p)" not in region
print("PNG SCREEN_GAMMA parsing contract passed")
