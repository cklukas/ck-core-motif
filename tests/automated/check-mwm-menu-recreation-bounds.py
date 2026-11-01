#!/usr/bin/env python3
"""Check MWM menu-widget recreation bounds its affected-specification list."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(encoding="utf-8")
start = source.index("static void RecreateMenuWidgets")
end = source.index("\n}\n", start) + 2
region = source[start:end]

assert "size_t count = 0, i;" in region
assert "if (count == SIZE_MAX)" in region
assert "count > SIZE_MAX / sizeof(*to_change)" in region
assert "count > UINT_MAX / sizeof(*to_change)" in region
assert "(Cardinal)(sizeof(MenuSpec *) * (size_t)count)" in region
assert "for (i = 0; i < count; ++i)" in region
print("PASS MWM menu-widget recreation bounds its list and Cardinal allocation")
