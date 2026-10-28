#!/usr/bin/env python3
"""Keep XmString component strlen results representable."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/XmString.c").read_text()
helper_start = source.index("set_component_string_length")
function_start = source.index("XmeStringGetComponent(_XmStringContext context")
body = source[function_start:]
assert "set_component_string_length(length, value, copy_data)" in body
assert "component_length > (size_t)UINT_MAX" in source[helper_start:function_start]
assert "XtFree((char *)*value);" in source[helper_start:function_start]
assert "*length = strlen(" not in body
print("XmString component lengths are checked before narrowing")
