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
assert "char8_item < char8_base || char8_item >= char8_end" in body
assert "memchr((const void *)char8_table_src[cnt], '\\0'," in body
assert "(size_t)(char8_item - char8_base)" in body
assert "for (cnt=0; cnt<vec_count; cnt++)" in body
print("Mrm Char8 vector span and embedded pointers are validated before rebasing")
