#!/usr/bin/env python3
"""Check Unicode line scans validate source and gap state before access."""

from pathlib import Path


source = Path("lib/Xm/TextStrSo.c").read_text()
start = source.index("SourceLineCharacter(XmSourceData data")
body = source[start:source.index("static unsigned long\nSourceNewlineCharacter", start)]

for guard in (
    "data == NULL",
    "data->ptr == NULL",
    "data->gap_start == NULL",
    "data->gap_end == NULL",
    "data->gap_end < data->gap_start",
    "data->widgets == NULL",
    "data->widgets[0] == NULL",
    "data->length > data->maxlength",
):
    assert guard in body
assert "SIZE_MAX / unit" in body
assert "gap_bytes > total_bytes - gap_offset" in body
assert "byte_offset > total_bytes - gap_offset" in body

print("Unicode source line character access is guarded")
