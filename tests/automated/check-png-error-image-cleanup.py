#!/usr/bin/env python3
"""Ensure PNG conversion reports unsupported depth and cleans failed images."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Png.c").read_text()
process = source[source.index("rc = png_process_image") : source.index("static double", source.index("rc = png_process_image"))]

assert "if (rc != 0)" in process
assert "XDestroyImage(*ximage);" in process
assert "*ximage = NULL;" in process
depth = source[source.index("} else { /* depth == 8 */") : source.index("    return 0;", source.index("} else { /* depth == 8 */"))]
assert "return 2;" in depth
print("PASS PNG conversion cleans failed images and rejects unsupported depth")
