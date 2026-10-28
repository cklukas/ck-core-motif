#!/usr/bin/env python3
"""Check inverse compound-text conversion validates XrmValue records."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/ResEncod.c").read_text()
public_start = source.index("XmCvtXmStringToText(")
public_end = source.index("    ok = cvtXmStringToText", public_start)
public = source[public_start:public_end]
helper_start = source.rindex("cvtXmStringToText(")
helper_end = len(source)
helper = source[helper_start:helper_end]

assert "from_val == NULL || to_val == NULL" in public
assert "if (from == NULL || to == NULL)" in helper
assert "return(False);" in helper

print("PASS inverse compound-text converters validate records")
