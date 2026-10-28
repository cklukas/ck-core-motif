#!/usr/bin/env python3
"""Ensure Mrm byte-string vectors fit their advertised storage span."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(
    encoding="utf-8"
)
start = text.index("Urm__CW_SafeCopyValue (long")
body = text[start:text.index("\n}\n", start)]
assert "reptype == MrmRtypeChar8Vector" in body
assert "vec_size < (size_t)vec_count * sizeof(String)" in body
assert "for (cnt=0; cnt<vec_count; cnt++)" in body
print("Mrm Char8 vector span is validated before iteration")
