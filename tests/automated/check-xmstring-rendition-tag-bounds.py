#!/usr/bin/env python3
"""Check rendition-tag extraction respects Xt's Cardinal allocation size."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/XmStringSeg.c").read_text(
    encoding="utf-8"
)

assert source.count("UINT_MAX / sizeof(XmStringTag)") >= 2
print("XmString rendition-tag extraction is bounded")
