#!/usr/bin/env python3
"""Check that MWM label parsing validates input and allocation size."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(encoding="utf-8")
start = source.rindex("static Boolean ParseWmLabel (WmScreenData")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "size_t labelLength;" in region
assert "if (menuItem == NULL || string == NULL)" in region
assert "labelLength = strlen((char *)string);" in region
assert "labelLength >= UINT_MAX" in region
assert "XtMalloc((Cardinal)labelLength + 1)" in region
assert "XtMalloc(labelLength + 1)" not in region
name_start = source.index("nameLength = (size_t)(endquote - lineP);")
name_end = source.index("    else\n", name_start)
name_region = source[name_start:name_end]
assert "nameLength >= UINT_MAX" in name_region
assert "XtMalloc((Cardinal)nameLength + 1)" in name_region
assert "XtMalloc(nameLength + 1)" not in name_region
print("PASS MWM label parsing validates input and allocation")
