#!/usr/bin/env python3
"""Check XmTabAttributesFree accepts an absent attribute record."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabList.c").read_text()
start = source.index("XmTabAttributesFree(XmTabAttributes attributes)")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "if (attributes == NULL)" in region
assert region.index("if (attributes == NULL)") < region.index("attributes->value_mode")
print("PASS TabList attribute-free guard")
