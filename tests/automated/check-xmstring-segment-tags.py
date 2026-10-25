#!/usr/bin/env python3
"""Check malformed rendition-tag tables are rejected before string copying."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringSeg.c").read_text()
start = source.index("CopyRenditionTags(")
body = source[start:source.index("/*", start + 20)]
assert "if (source == NULL)" in body
assert "if (source[i] == NULL)" in body
assert "if (copy_data)" in body
