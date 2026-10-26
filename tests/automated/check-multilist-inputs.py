#!/usr/bin/env python3
"""Check MultiList public selection APIs reject null widgets."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "MultiList.c").read_text(encoding="utf-8")
public_start = source.index("XmMultiListGetSelectedRows")
public_end = source.index("/* aliases of deprecated", public_start)
region = source[public_start:public_end]
assert region.count("if (w == NULL)") >= 9
assert "if (w == NULL) {" in region
assert "*num_rows = 0;" in region
print("PASS MultiList selection APIs validate null widgets")
