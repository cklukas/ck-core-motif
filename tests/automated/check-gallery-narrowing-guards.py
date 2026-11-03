#!/usr/bin/env python3
"""Verify gallery geometry, screenshot, and toolbar narrowing guards."""

from pathlib import Path


root = Path(__file__).resolve().parents[2]
gallery = (root / "demos" / "programs" / "ckmotif-gallery" / "gallery.c").read_text(
    encoding="utf-8"
)
examples = (
    root / "demos" / "programs" / "ckmotif-gallery" / "gallery_examples.c"
).read_text(encoding="utf-8")
audit = (root / "demos" / "programs" / "ckmotif-gallery" / "gallery_audit.h").read_text(
    encoding="utf-8"
)

assert "static Dimension dimension_from_extent(unsigned long extent)" in gallery
assert "static Position position_from_extent(unsigned long extent)" in gallery
assert "Dimension text_width = available > inset" in gallery
assert "unsigned long inset = 2UL * ((unsigned long)margin + shadow + highlight)" in gallery
assert "XmNy, position_from_extent(height)" in gallery
assert "dimension_from_extent(height ? height : 1UL)" in gallery
assert "(unsigned long)preferred.width + 2UL * (unsigned long)padding" in gallery
assert "if (expected_x < 0) expected_x = 0;" in gallery
assert "if (expected_y < 0) expected_y = 0;" in gallery
assert "used > maximum || length > maximum - used" in gallery
assert "XGetPixel(image, (int)x, (int)y)" in gallery
assert "(long)XInternAtom" in gallery
assert "XGetGeometry(XtDisplayOfObject(owner),probe" in examples
assert "&pixmap_depth) != 0" in examples
assert "static const Boolean overflow_modes[] = {False, True};" in examples
assert "if (!button) continue;" in examples
assert "a.width<=0||a.height<=0" in audit
assert "(size_t)a.width>SIZE_MAX/3" in audit

print("gallery checks geometry narrowing, screenshot dimensions, and absent toolbar buttons")
