#!/usr/bin/env python3
"""Ensure XPM data extensions track remaining output capacity per line."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text(
    encoding="utf-8"
)
start = source.index("static int\nCreateExtensions(")
body = source[start:]
assert "size_t used;" in body
assert "used = (size_t)offset;" in body
assert "(size_t)data_size - used" in body
assert "line_length = strlen(*line) + 1;" in body
assert "line_length > (size_t)data_size - used" in body
assert 'sizeof("XPMENDEXT") > (size_t)data_size - used' in body
assert 'strcpy(*dataptr, *line)' not in body
print("PASS XPM data extension writes track remaining capacity")
