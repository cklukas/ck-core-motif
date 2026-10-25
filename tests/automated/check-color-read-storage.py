#!/usr/bin/env python3
"""Check that ColorObj read-side access validates per-screen storage."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text(
    encoding="utf-8"
)
start = source.index("XmeGetColorObjData(")
end = source.index("\n}\n", start)
region = source[start:end]
assert "colorUse == NULL" in region
assert "colors == NULL" in region
assert region.index("colorUse == NULL") < region.index("colorUse[screen_num]")
assert region.index("colors == NULL") < region.index("colors[screen_num]")
print("PASS ColorObj read-side storage is validated")
