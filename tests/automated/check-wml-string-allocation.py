#!/usr/bin/env python3
"""Check WML's shared string-copy helper handles allocation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "tools" / "wml" / "wmlutils.c").read_text(
    encoding="utf-8"
)
start = source.index("char *wmlAllocateString")
end = source.index("void wmlUpperCaseString", start)
region = source[start:end]

assert "length = strlen(stg);" in region
assert "dynstg = (char *) malloc (length + 1);" in region
assert "if (dynstg == NULL)" in region
assert "memcpy(dynstg, stg, length + 1);" in region
assert "strcpy (dynstg, stg);" not in region

print("PASS WML string helper rejects allocation failure before copying")
