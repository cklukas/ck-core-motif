#!/usr/bin/env python3
"""Check MWM distinguishes an absent bitmap name from an unusable one."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmImage.c").read_text(
    encoding="utf-8"
)
start = source.index("int GetBitmapIndex")
end = source.index("} /* END OF FUNCTION GetBitmapIndex */", start)
region = source[start:end]

guard = "if (path == NULL && name != NULL && *name != '\\0')"
assert guard in region
assert region.index(guard) < region.index("for (n = 0, bitmapc")
assert "return (-1);" in region[region.index(guard):region.index("for (n = 0, bitmapc")]
assert "A missing name selects the builtin icon" in region
print("PASS MWM rejects unusable bitmap paths without selecting the builtin icon")
