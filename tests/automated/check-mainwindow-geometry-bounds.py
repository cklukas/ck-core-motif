#!/usr/bin/env python3
"""Check MainWindow geometry-manager dimension conversions are bounded."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "MainW.c").read_text(
    encoding="utf-8"
)

assert "#include <limits.h>" in source
assert "static Dimension\nMainWClampDimension(long long value)" in source
assert "if (value > USHRT_MAX)" in source

start = source.index("static XtGeometryResult \nGeometryManager(")
body = source[start:source.index("\f", start)]
assert "newWidth = MainWClampDimension((long long)request->width +" in body
assert "2 * ((long long)bw + mw->mwindow.margin_width)" in body
assert "newHeight = MainWClampDimension(parent_height);" in body
assert "newWidth = request->width + 2 * (bw + mw->mwindow.margin_width);" not in body

get_size = source[source.index("static void \nGetSize("):source.index("\f", source.index("static void \nGetSize("))]
assert "MyXpad = MainWClampDimension(" in get_size
assert "MyYpad = MainWClampDimension(" in get_size
assert get_size.count("newWidth = MainWClampDimension(") >= 2
assert get_size.count("newHeight = MainWClampDimension(") >= 8

print("PASS MainWindow geometry-manager dimensions are bounded")
