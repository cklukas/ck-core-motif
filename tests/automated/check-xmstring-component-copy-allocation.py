#!/usr/bin/env python3
"""Check that XmString component-copy allocation failures end iteration safely."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("case XmCHARSET_TEXT:\n\tcase XmMULTIBYTE_TEXT:")
region = source[start:start + 1200]

assert "SIZE_MAX - sizeof(wchar_t)" in region
assert "if (!tmp)" in region
assert region.count("return XmSTRING_COMPONENT_END;") == 2
assert "_XmStrContError(context) = True" in region
print("PASS XmString component-copy allocation failures end iteration safely")
