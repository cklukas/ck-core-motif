#!/usr/bin/env python3
"""Check composite-resource copying respects Xt's Cardinal size limit."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/BaseClass.c").read_text(
    encoding="utf-8"
)

assert "num_comp_resources > UINT_MAX / sizeof(XtResource)" in source
assert "tmp_resources = (XtResourceList)" in source
assert "if (!tmp_resources)" in source
print("BaseClass resource-copy allocation is bounded")
