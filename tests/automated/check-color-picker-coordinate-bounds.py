#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/ColorPicker.c").read_text()
s = "".join(source.split())
assert "MAX(1,MIN(value,SHRT_MAX))" in s
assert "width=MIN(p->core.width,SHRT_MAX),height=MIN(p->core.height,SHRT_MAX)" in s
print("PASS color-picker-coordinate-bounds")
