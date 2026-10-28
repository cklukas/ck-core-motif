#!/usr/bin/env python3
"""Check public DataField conversion APIs reject unterminated output."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()

set_start = source.index("XmDataFieldSetStringWcs(\n        Widget w")
set_end = source.index("void \n", set_start)
set_body = source[set_start:set_end]
assert "result == (size_t)-1" in set_body
assert "result >= (size_t)(num_chars + 1)" in set_body
assert "tmp[result] = '\\0'" in set_body

get_start = source.index("XmDataFieldGetSelection(\n        Widget w")
get_end = source.index("wchar_t *", get_start)
get_body = source[get_start:get_end]
assert "length == (size_t)-1 || length >= capacity" in get_body
assert "XtFree(value);" in get_body
assert "return NULL;" in get_body

print("PASS DataField public conversion bounds")
