#!/usr/bin/env python3
"""Check that Xmos mask-name construction copies the measured prefix exactly."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text()
start = source.index("_XmOSGenerateMaskName(")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "memcpy(maskNameBuf, imageName, len);" in region
assert "strncpy(maskNameBuf, imageName, len);" not in region
print("Xmos mask-name prefix copying is exact-length")
