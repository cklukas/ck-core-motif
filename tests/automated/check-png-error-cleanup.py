#!/usr/bin/env python3
"""Check that libpng long-jump failures release decoded allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Png.c").read_text()
assert "volatile unsigned char *allocated_image = NULL;" in source
assert "volatile png_bytepp allocated_rows = NULL;" in source
branch = source[source.index("if (setjmp(png_jmpbuf(png_ptr)))"):source.index("png_init_io", source.index("if (setjmp(png_jmpbuf(png_ptr)))"))]
assert "free((void *)allocated_rows);" in branch
assert "free((void *)allocated_image);" in branch
assert "*image_data = NULL;" in branch
assert source.index("png_read_end(png_ptr, NULL);") < source.index("free(row_pointers);")
print("PNG decoder failures release decoded image and row storage")
