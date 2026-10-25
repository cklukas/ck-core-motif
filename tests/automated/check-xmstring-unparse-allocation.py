#!/usr/bin/env python3
"""Check that XmString unparse conversion growth fails closed."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("static void\nunparse_text(")
end = source.index("\n/*\n * unparse_is_plausible", start)
region = source[start:end]

assert region.count("if (!null_text)") == 2
assert region.count("if (!new_result)") == 3
assert "SIZE_MAX / MB_CUR_MAX" in region
assert "SIZE_MAX - (size_t)c_length" in region
assert region.count("(Cardinal)~0") == 5
assert "c_length + sizeof(wchar_t)" in region
assert "c_length + 1" in region
print("PASS XmString unparse allocations and size arithmetic are guarded")
