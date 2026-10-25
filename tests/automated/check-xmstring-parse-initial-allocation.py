#!/usr/bin/env python3
"""Check XmStringParseText propagates initial component allocation failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmStringParseText(")
end = source.index("\n\f", start)
region = source[start:end]

assert "result = XmStringComponentCreate(tag_type" in region
assert "if (result == NULL)" in region
assert region.index("if (result == NULL)") < region.index("/* Did the user provide")
print("PASS XmStringParseText guards initial component allocation")
