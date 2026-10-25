#!/usr/bin/env python3
"""Check that mwm DISPLAY environment storage is sized from the display name."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmInitWs.c").read_text()
start = source.index("void\nInitWmDisplayEnv")
end = source.index("} /* END OF FUNCTION  InitWmDisplayEnv */", start)
region = source[start:end]
assert "char buffer[256]" not in region
assert "char displayName[256]" not in region
assert "displayNameLength = strlen(pDisplayName)" in region
assert "displayNameLength > SIZE_MAX - strlen(\"DISPLAY=\") - 1" in region
assert "displayLength > UINT_MAX" in region
assert "XtMalloc((Cardinal)displayLength)" in region
assert "XtMalloc((unsigned int)displayLength)" not in region
assert "snprintf(wmGD.displayString, displayLength" in region
assert "strcpy(displayName" not in region
print("mwm DISPLAY environment storage is dynamically bounded")
