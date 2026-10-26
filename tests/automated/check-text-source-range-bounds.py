#!/usr/bin/env python3
"""Ensure legacy Text source reads cannot silently truncate ranges."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text(
    encoding="utf-8"
)

read_start = source.index("ReadSource(XmTextSource source,")
read_end = source.index("\nvoid\n_XmTextValidate", read_start)
read_region = source[read_start:read_end]
assert "available > (XmTextPosition)(INT_MAX - position)" in read_region

get_start = source.index("_XmStringSourceGetString(XmTextWidget tw,")
get_end = source.index("\n\nstatic void \nAddWidget", get_start)
get_region = source[get_start:get_end]
assert get_region.count("if (pos != to)") == 2
assert get_region.count("XtFree(buf);") >= 2

print("Text source range advances and incomplete reads are bounded")
