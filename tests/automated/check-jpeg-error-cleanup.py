#!/usr/bin/env python3
"""Check that libjpeg failures release an already decoded buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Jpeg.c").read_text()
branch = source[source.index("if ((rc = setjmp(jerr.setjmp_buffer)))"):source.index("jpeg_create_decompress", source.index("if ((rc = setjmp(jerr.setjmp_buffer)))"))]
assert "free(*image_data);" in branch
assert "*image_data = NULL;" in branch
print("JPEG decoder failures release decoded image storage")
