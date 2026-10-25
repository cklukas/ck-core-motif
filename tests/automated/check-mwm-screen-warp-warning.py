#!/usr/bin/env python3
"""Check that the unmanaged-screen warning does not use a fixed buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmFunction.c").read_text()
start = source.index("Boolean F_Screen")
end = source.index("\n    if (ACTIVE_PSD->screen == scr)", start)
region = source[start:end]
assert "char pch[80]" not in region
assert "sprintf(pch" not in region
assert "snprintf(NULL, 0, format, scr)" in region
assert "(unsigned int)messageLength < UINT_MAX" in region
assert "XtMalloc((Cardinal)messageLength + 1)" in region
assert "XtMalloc((unsigned int)messageLength + 1)" not in region
assert "XtFree(message);" in region
print("mwm unmanaged-screen warning is dynamically bounded")
