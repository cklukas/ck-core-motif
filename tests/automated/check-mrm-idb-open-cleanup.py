#!/usr/bin/env python3
"""Check low-level Mrm IDB opens close descriptors on allocation failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIfile.c").read_text(encoding="utf-8")
first = source.index("if (*file_id==0)")
first_end = source.index("a_file =", first)
first_region = source[first:first_end]
assert "close (file_desc)" in first_region

second = source.index("if (a_file->name==0)")
second_end = source.index("a_file->file_desc", second)
second_region = source[second:second_end]
assert "close (file_desc)" in second_region
print("Mrm IDB low-level opens close descriptors on allocation failure")
