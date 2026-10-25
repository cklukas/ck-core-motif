#!/usr/bin/env python3
"""Check ToggleButton select-color conversion validates XrmValue payloads."""

from pathlib import Path


source = Path("lib/Xm/ToggleB.c").read_text()
assert source.count("value.addr != NULL && value.size == sizeof(Pixel)") == 2
assert "assert(value.size == sizeof(Pixel));" not in source

print("ToggleButton color conversion validates Pixel payloads")
