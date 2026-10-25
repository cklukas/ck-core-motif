#!/usr/bin/env python3
"""Check bounded ColorObj property parsing."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text(encoding="utf-8")
start = text.index("\nFetchPixelData(Widget w")
end = text.index("static Boolean", start)
region = text[start:end]
for needle in (
    "value_length > UINT_MAX - 1",
    "bounded_value = (char *)XtMalloc((Cardinal)value_length + 1)",
    "memcpy(bounded_value, value, (size_t)value_length)",
    "bounded_value[value_length] = '\\0'",
    'sscanf (bounded_value, "%x_%n"',
    'sscanf (&(bounded_value[count]), "%lx_%lx_%lx_%lx_%lx_%n"',
    "XtFree(bounded_value)",
):
    assert needle in region, f"missing bounded property parser contract: {needle}"
assert 'sprintf(tmp, "%x_"' not in region
assert 'sprintf(tmp,"%lx_' not in region
print("ColorObj property parsing is bounded")
