#!/usr/bin/env python3
"""Check public tab value APIs reject null handles safely."""

from pathlib import Path


source = Path("lib/Xm/XmTabList.c").read_text()

start = source.index("XmTabGetValues(")
end = source.index("XmTabSetValue(", start)
get_values = source[start:end]
set_start = end
set_end = source.index("_XmTabCopy(", set_start)
set_value = source[set_start:set_end]

assert "if (tab == NULL)" in get_values
assert "return 0.0;" in get_values
assert "if (tab == NULL)" in set_value
assert "return;" in set_value

print("tab value APIs guard null handles")
