#!/usr/bin/env python3
"""Check that mwm screen display formatting has no fixed 256-byte buffers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmInitWs.c").read_text()
start = source.index("void\nInitWmScreen")
end = source.index("} /* END OF FUNCTION  InitWmScreen */", start)
region = source[start:end]
assert "#define LENCBUFFER 256" not in region
assert "char buffer[LENCBUFFER]" not in region
assert "char displayName[LENCBUFFER]" not in region
assert "displayNameLength = strlen(pDisplayName) + 1" in region
assert "XtMalloc((unsigned int)(displayNameLength + 64))" in region
assert "XtFree(buffer);" in region
assert "XtFree(displayName);" in region
print("mwm screen display formatting is dynamically bounded")
