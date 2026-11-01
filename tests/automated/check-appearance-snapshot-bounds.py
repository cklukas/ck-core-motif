#!/usr/bin/env python3
"""Check appearance transaction snapshots bound count-based allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Appearance.c").read_text()
start = source.index("static Boolean\nappearance_apply_subtree(Display *display, Widget root,", source.index("_XmRegisterAppearanceChange"))
body = source[start:source.index("Boolean\n_XmApplyAppearanceChange", start)]
assert "size_t count = 0;" in body
assert "Cardinal count = 0;" not in body
assert "SIZE_MAX / sizeof(*snapshot)" in body
assert "UINT_MAX / sizeof(*snapshot)" in body
assert "SIZE_MAX / sizeof(*plans)" in body
assert "for (size_t i = 0; i < count; ++i)" in body
assert "(Cardinal)(count * sizeof(*snapshot))" in body
assert "return False;" in body
print("PASS appearance transaction snapshot allocation bounds")

inset_start = source.index("_XmAppearanceInsetArrow(")
inset_end = source.index("\n/* Gadget constructors", inset_start)
inset = source[inset_start:inset_end]
assert "if (widget == NULL || x == NULL || y == NULL ||" in inset
assert "width == NULL || height == NULL)" in inset
assert "long long next_x" in inset and "long long next_y" in inset
assert "next_x < SHRT_MIN" in inset and "next_x > SHRT_MAX" in inset
assert "next_y < SHRT_MIN" in inset and "next_y > SHRT_MAX" in inset
