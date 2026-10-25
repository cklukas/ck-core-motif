#!/usr/bin/env python3
"""Check that WSM backdrop-path construction is checked and bounded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmInitWs.c").read_text(
    encoding="utf-8"
)
start = source.index("InsureDefaultBackdropDir(char **ppchBackdropDirs)", source.index("InsureDefaultBackdropDir(char **ppchBackdropDirs)") + 1)
end = source.index("#endif /* WSM */", start)
function = source[start:end]
assert "snprintf(NULL, 0" in function
assert "formattedLength < 0" in function
assert "(size_t)formattedLength >= UINT_MAX" in function
assert "XtMalloc((Cardinal)formattedLength + 1)" in function
assert "Insufficient memory for backdrop search path" in function
assert "strcpy (tmpptr" not in function
assert "len = strlen" not in function
print("WSM backdrop search paths use checked two-pass formatting")
