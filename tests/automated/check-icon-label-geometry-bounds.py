#!/usr/bin/env python3
"""Check bounded XmIconLabel geometry calculations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconLabel.c").read_text(encoding="utf-8")
start = source.index("static Dimension text_width")
end = source.index("static Boolean pixmap_size", start)
region = source[start:end]
assert "length > (USHRT_MAX / 8)" in region
assert "(Dimension)(length * 8)" in region
dimensions = source.index("static void dimensions")
end_dimensions = source.index("static void Initialize", dimensions)
region = source[dimensions:end_dimensions]
assert "total_width > USHRT_MAX" in region
print("PASS icon-label geometry is bounded")
