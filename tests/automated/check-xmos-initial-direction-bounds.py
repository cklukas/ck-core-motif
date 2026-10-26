#!/usr/bin/env python3
"""Check Xmos initial-character direction output bounds and null inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text()
start = source.index("Status\n_XmOSGetInitialCharsDirection")
end = source.index("XmDirection\n_XmOSGetCharDirection", start)
function = source[start:end]

assert "size_t length;" in function
assert "characters == NULL || num_bytes == NULL || direction == NULL" in function
assert "length > SIZE_MAX / sizeof(wchar_t)" in function
assert "length * sizeof(wchar_t) > UINT_MAX" in function
assert "if (length > UINT_MAX)" in function
assert "*num_bytes = (unsigned int)length;" in function
assert "static size_t\nWcslen" in source
assert "if (ptr == NULL)" in source
assert "return (size_t)(ptr - wcs);" in source

print("PASS Xmos initial-character direction bounds")
