#!/usr/bin/env python3
"""Check Frame layout calculations saturate X11 dimensions and positions."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Frame.c").read_text(
    encoding="utf-8"
)

assert "#include <limits.h>" in source
assert "static Dimension\nFrameClampDimension(long long value)" in source
assert "if (value > USHRT_MAX)" in source
assert "if (value < 0)" in source

size_start = source.index("static void \nCalcFrameSize(")
size = source[size_start:source.index("static void \nCalcWorkAreaSize(", size_start)]
assert size.count("FrameClampDimension(") >= 3

configure_start = source.index("static void \nConfigureChildren(")
configure = source[configure_start:source.index("static void \nCalcFrameSize(", configure_start)]
assert "spacing = FrameClampDimension((long long)shadowThickness +" in configure

extent_start = source.index("static void \nCalcTitleExtent(")
extent = source[extent_start:source.index("static void \nConstraintInitialize(", extent_start)]
assert "Dimension total = FrameClampDimension((long long)titleHeight +" in extent
assert extent.count("FrameClampDimension(") >= 4
assert "FrameClampPosition((long long)shadowThickness / 2 -" in extent
assert "FrameClampPosition((long long)base + titleBorder -" in extent
assert "sy = FrameClampPosition(total);" in extent
assert "ty = FrameClampPosition(shadowThickness);" in extent

print("PASS Frame layout dimensions and positions are bounded")
