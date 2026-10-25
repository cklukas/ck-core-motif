#!/usr/bin/env python3
"""Check that Mwm menu command streams account for separator and NUL bytes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean ParseClientCommand")
region = source[start:source.index("for (;;)", start)]
assert "prefix_length + suffix_length + 2" in region
assert "prefix_length + suffix_length > SIZE_MAX - 2" in region
assert "XtMalloc((Cardinal)stream_length)" in region
print("Mwm menu command streams reserve separator and terminator bytes")
