#!/usr/bin/env python3
"""Check that DataField cursor pixmap names have bounded storage."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("df_MakeIBeamStencil")
end = source.index("/* The IBeam Stencil", start)
ibeam = source[start:end]
assert "char pixmap_name[64]" in ibeam
assert ibeam.count("snprintf(pixmap_name, sizeof(pixmap_name)") == 2
assert "sprintf(pixmap_name" not in ibeam

start = source.index("df_MakeAddModeCursor( tf, line_width )")
end = source.index("\n}", start) + 2
add_mode = source[start:end]
assert "char pixmap_name[64]" in add_mode
assert "snprintf(pixmap_name, sizeof(pixmap_name)" in add_mode
assert "sprintf(pixmap_name" not in add_mode
print("DataField cursor pixmap names are bounded")
