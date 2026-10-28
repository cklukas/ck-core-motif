#!/usr/bin/env python3
"""Check XPM file-to-buffer reads propagate close errors."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmRdFToBuf.c").read_text()
start = source.index("file_size = (size_t)len;")
region = source[start : source.index("ptr[len]", start)]

assert "close_status = fclose(fp);" in region
assert "close_status != 0" in region
assert "file_size = (size_t)len;" in region
assert "fread(ptr, 1, file_size, fp)" in region
assert region.index("close_status = fclose(fp);") < region.index("close_status != 0")
print("XPM file-to-buffer reads propagate close errors")
