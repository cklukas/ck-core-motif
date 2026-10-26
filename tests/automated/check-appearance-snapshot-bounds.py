#!/usr/bin/env python3
"""Check appearance transaction snapshots bound count-based allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Appearance.c").read_text()
start = source.index("Boolean\n_XmApplyAppearanceChange")
body = source[start:source.index("static Boolean\nprofile_from_name", start)]
assert "SIZE_MAX / sizeof(*snapshot)" in body
assert "UINT_MAX / sizeof(*snapshot)" in body
assert "SIZE_MAX / sizeof(*plans)" in body
assert "return False;" in body
print("PASS appearance transaction snapshot allocation bounds")
