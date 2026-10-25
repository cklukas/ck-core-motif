#!/usr/bin/env python3
"""Check _XmFilterArgs allocation failure and byte bounds."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmExtUtil.c").read_text()
start = source.index("_XmFilterArgs(")
end = source.index("\n/*\tFunction Name: _XmSetValuesOnChildren", start)
region = source[start:end]

assert "SIZE_MAX / sizeof(Arg)" in region
assert "UINT_MAX / sizeof(Arg)" in region
assert "if (fargs == NULL)" in region
assert region.index("if (fargs == NULL)") < region.index("*filtered_args = fargs")
print("Xm argument filtering handles allocation failure")
