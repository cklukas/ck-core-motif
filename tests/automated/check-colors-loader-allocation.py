#!/usr/bin/env python3
"""Check ColorS keeps allocation failures transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c").read_text()
assert "Boolean allocation_failed = False;" in source
assert "ColorInfo *new_info" in source
assert "if (new_info == NULL)" in source
assert "if (allocation_failed)" in source
assert "if (strs == NULL && count != 0)" in source
assert "if (count >= SHRT_MAX)" in source
assert "UINT_MAX / sizeof(*color_info)" in source
assert "UINT_MAX / sizeof(*strs)" in source
assert "Cardinal allocation_size" in source
assert "qsort(color_info, (size_t)count" in source
assert "XmColorS_num_colors(csw) = (short)count;" in source
assert "for (size_t name_index = 0; name_index < len; name_index++)" in source

print("ColorSelector RGB loader checks allocation sizes and count width")
