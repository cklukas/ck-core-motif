#!/usr/bin/env python3
"""Ensure Mrm color-table arguments are sized and copied into the record."""

from pathlib import Path


writer = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c").read_text(
    encoding="utf-8"
)
start = writer.index("UrmCWRSetArgValue (")
body = writer[start:writer.index("\n}\n", start)]
size_pos = body.index("color_table_size = UrmColorTableSize(src_ct)")
copy_pos = body.index("memcpy (dst_ct, src_ct, color_table_size)")
assert size_pos < copy_pos
assert "color_table_size > USHRT_MAX" in body[size_pos:copy_pos]

helper = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmicon.c").read_text(
    encoding="utf-8"
)
helper_start = helper.index("UrmColorTableSize (RGMColorTablePtr")
helper_body = helper[helper_start:helper.index("\n}\n", helper_start)]
assert "ctable->count < URMColorTableUserMin" in helper_body
assert "ctable->count - 1" in helper_body
print("Mrm color-table arguments are count-sized and copied")
