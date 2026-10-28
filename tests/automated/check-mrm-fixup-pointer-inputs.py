#!/usr/bin/env python3
"""Ensure Mrm value fixup validates metadata and unit-value pointers."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(
    encoding="utf-8"
)
start = text.index("Urm__CW_FixupValue (long")
body = text[start:text.index("\n}\n\n\n", start)]
assert "file_id == NULL" in body
assert "file_id->db_version == NULL" in body
assert body.count("if (val == 0)") >= 2
print("Mrm value fixup validates metadata and unit pointers")
