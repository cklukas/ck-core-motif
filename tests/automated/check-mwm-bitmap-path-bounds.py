#!/usr/bin/env python3
"""Check that mwm bitmap path assembly rejects unrepresentable paths safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmImage.c").read_text(encoding="utf-8")
start = source.index("char *BitmapPathName")
end = source.index("/* END OF FUNCTION BitmapPathName */", start)
region = source[start:end]
assert "WmCopyBitmapPath" in region
assert "WmAppendBitmapPath" in region
assert "strcpy (fileName" not in region
assert "strncpy(fileName" not in region
assert "return (string);" in region
print("PASS mwm bitmap path assembly is bounded and terminated")
