#!/usr/bin/env python3
"""Check FontS fixed formatting buffers use bounded operations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text()
assert 'snprintf(point_size, sizeof(point_size)' in source
assert 'snprintf(res_x, sizeof(res_x)' in source
assert 'snprintf(res_y, sizeof(res_y)' in source
assert 'snprintf(name, sizeof(name), "button_%d"' in source
assert 'snprintf(buf, sizeof(buf), "%d", cf->point_size / POINT_DIVIDE)' in source
assert 'snprintf(buf, sizeof(buf), "*button_%d"' in source
assert 'sprintf(point_size' not in source
assert 'sprintf(name, "button_%d"' not in source
print("PASS FontS uses bounded formatting for selector names")
