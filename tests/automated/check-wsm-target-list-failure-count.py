#!/usr/bin/env python3
"""Check that WSM target-list failures cannot leave a stale count."""

from pathlib import Path


source = (
    Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "recv.c"
).read_text(encoding="utf-8")
start = source.index("WSMGetTargetList(Widget w, Boolean include_defaults")
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "if (len_ret == NULL)" in function
assert "*len_ret = 0;" in function
assert "if (w == NULL)" in function
assert function.index("*len_ret = 0;") < function.index("if (w == NULL)")
assert "if (disp_info == NULL)" in function
assert "*len_ret > UINT_MAX / sizeof(*list)" in function
assert "XtMalloc((Cardinal)(sizeof(*list) * (*len_ret)))" in function

print("WSM target-list failure clears the returned count")
