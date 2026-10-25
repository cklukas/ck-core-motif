#!/usr/bin/env python3
"""Check Container layout avoids division by zero before cell metrics exist."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()

snap_start = source.index("SnapCwid(")
snap_start = source.index("static  XPoint *", snap_start - 80)
snap_end = source.index("\n}\n", snap_start) + 2
snap = source[snap_start:snap_end]
assert "if (cell_width <= 0 || cell_height <= 0)" in snap
assert snap.index("if (cell_width <= 0 || cell_height <= 0)") < snap.index("/ cell_width")
assert snap.index("if (cell_width <= 0 || cell_height <= 0)") < snap.index("/ cell_height")
assert "return point;" in snap

layout_start = source.index("static  void\nLayoutSpatial(")
layout_end = source.index("\n}\n", layout_start) + 2
layout = source[layout_start:layout_end]
assert "if (cell_width <= 0 || cell_height <= 0)" in layout
assert layout.index("if (cell_width <= 0 || cell_height <= 0)") < layout.index("/ cell_width")
assert layout.index("if (cell_width <= 0 || cell_height <= 0)") < layout.index("/ cell_height")
assert "return;" in layout
print("PASS Container zero-cell guards")
