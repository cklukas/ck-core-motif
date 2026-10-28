#!/usr/bin/env python3
"""Check ImageCache uses the shared bounded print-resolution parser."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
region = source[source.index('"default-printer-resolution"') - 260:
                source.index('"default-printer-resolution"') + 320]
assert "_XmParsePrintResolution(resolution)" in region
assert "XFree(resolution)" in region
assert "atoi(" not in region
assert "extern int _XmParsePrintResolution" in (
    Path(__file__).parents[2] / "lib" / "Xm" / "XmI.h"
).read_text()
print("ImageCache uses checked, shared print-resolution parsing")
