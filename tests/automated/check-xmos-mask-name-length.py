#!/usr/bin/env python3
"""Check Xmos mask-name generation keeps prefix lengths in size_t."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text()
start = source.index("_XmOSGenerateMaskName(")
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "size_t\tlen;" in function
assert "if (imageName == NULL || maskNameBuf == NULL)" in function
assert "len = (size_t)(suffix - imageName - 1);" in function
assert "memcpy(maskNameBuf, imageName, len);" in function
assert "(int)(suffix - imageName)" not in function

print("PASS Xmos mask-name generation preserves size_t prefix length")
