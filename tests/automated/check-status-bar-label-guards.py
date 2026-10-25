#!/usr/bin/env python3
"""Check safe status-bar label composition."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "StatusBar.c").read_text(encoding="utf-8")
start = source.index("static XmString display_string")
end = source.index("static void free_items", start)
region = source[start:end]

assert "if (!label)" in region
assert "SIZE_MAX - strlen(progress) - 1" in region
assert "if (size > UINT_MAX)" in region
assert "if (!text)" in region
print("PASS status-bar labels validate conversion and allocation")
