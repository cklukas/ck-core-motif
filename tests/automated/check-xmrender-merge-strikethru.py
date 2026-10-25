#!/usr/bin/env python3
"""Check MergeInto inherits an unspecified strikethrough resource."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("static void\nMergeInto(")
end = source.index("\n\n/* Make a Clone", start)
region = source[start:end]

assert "if (_XmRendUnderlineType(toRend) == XmAS_IS)" in region
assert "if (_XmRendStrikethruType(toRend) == XmAS_IS)" in region
print("PASS MergeInto independently inherits underline and strikethrough")
