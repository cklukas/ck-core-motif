#!/usr/bin/env python3
"""Ensure XmString pattern matching checks wide-character conversion."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/XmString.c").read_text()
start = source.index("static Boolean\nmatch_pattern")
end = source.index("\n/*\n * parse_unmatched", start)
body = source[start:end]
assert body.count("wctomb(mb_text") == 2
assert body.count("wctomb(mb_pattern") == 2
assert body.count("< 0") >= 4
print("XmString pattern matching rejects failed wide-character conversions")
