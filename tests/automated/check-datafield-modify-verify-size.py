#!/usr/bin/env python3
"""Check DataField modify-verify temporary sizes are bounded."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("if (XmTextF_modify_verify_callback(new_tf)")
end = source.index("\n\t  if (free_insert)", start)
body = source[start:end]
assert "temp_capacity" in body
assert "SIZE_MAX / (size_t)XmTextF_max_char_size(new_tf) - 1" in body
assert "temp_capacity > UINT_MAX" in body
assert "XtMalloc((Cardinal)temp_capacity)" in body
assert "wcstombs(temp, XmTextF_wc_value(new_tf)," in body

print("DataField modify-verify temporary sizes are bounded")
