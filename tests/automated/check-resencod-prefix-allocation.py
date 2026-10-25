#!/usr/bin/env python3
"""Check custom compound-text prefixes validate size and allocation failures."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text()
start = source.index("/* Must be a non-standard character set! */")
body = source[start:source.index("/* Now copy in the text */", start)]
assert "size_t           tag_len;" in body
assert "size_t           allocation_size;" in body
assert "tag_len > (size_t)UINT_MAX - 7U" in body
assert "tag_len > SIZE_MAX - (size_t)*outlen - 8" in body
assert "temp = (unsigned char *) XtMalloc(allocation_size);" in body
assert "if (temp == NULL)" in body
assert "return(False);" in body
print("PASS custom compound-text prefix allocation is bounded")
