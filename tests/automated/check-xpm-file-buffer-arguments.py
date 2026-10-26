#!/usr/bin/env python3
"""Check XPM file-buffer helpers reject null public arguments."""
from pathlib import Path

read_source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmRdFToBuf.c").read_text()
write_source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmWrFFrBuf.c").read_text()
assert "filename == NULL || buffer_return == NULL" in read_source
assert "filename == NULL || buffer == NULL" in write_source
assert "return XpmFileInvalid;" in read_source
assert "return XpmFileInvalid;" in write_source
print("PASS XPM file-buffer helpers validate public arguments")
