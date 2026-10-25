#!/usr/bin/env python3
"""Ensure WSM data OOM paths consume encoded records."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
assert "SkipWinDataRec(data_ptr, attr);" in text
assert text.count("(void) UnpackCARD8(data_ptr);") >= 2
assert text.count("(void) UnpackCARD16(data_ptr);") >= 2
assert text.count("(void) UnpackCARD32(data_ptr);") >= 2
print("WSM data OOM paths preserve stream alignment")
