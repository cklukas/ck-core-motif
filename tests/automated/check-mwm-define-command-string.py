#!/usr/bin/env python3
"""Check that MWM DefineCommand bounds and releases decoded strings."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmCmd.c").read_text()
helper_start = source.index("UnpackCommandString")
helper_end = source.index("\n}\n", helper_start)
helper = source[helper_start:helper_end]
start = source.index("void\nDefineCommand")
end = source.index("/*----------------------------------------------------------------------*\n |                            IncludeCommand", start)
region = source[start:end]

assert "*remaining < 2" in helper
assert "length > *remaining - 2" in helper
assert "memcpy(result, bytes + 2, length);" in helper
assert "len < (unsigned long)(3 * sizeof(CARD32))" in region
assert "UnpackCommandString(&data, &remaining)" in region
assert "Truncated string data passed to DefineCommand." in region
assert region.count("XtFree(name);") == 2
assert region.count("XtFree(defaultLabel);") == 2
assert "UnpackString(&data)" not in region
print("MWM DefineCommand string contract passed")
