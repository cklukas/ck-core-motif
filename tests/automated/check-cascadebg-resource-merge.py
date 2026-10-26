#!/usr/bin/env python3
"""Check CascadeButton resource-list merging respects Xt's size limit."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/CascadeBG.c").read_text(
    encoding="utf-8"
)

assert "UINT_MAX / sizeof(XtResource)" in source
assert "merged_list = (XtResource *)XtMalloc" in source
assert "if (merged_list == NULL)" in source
print("CascadeButton resource merge allocation is bounded")
