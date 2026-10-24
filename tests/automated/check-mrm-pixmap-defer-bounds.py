#!/usr/bin/env python3
"""Check Mrm deferred pixmap storage stays within its fixed array."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(encoding="utf-8")
start = source.index("\nUrm__CW_CreateArglist (")
end = source.index("\n\n\f", start)
region = source[start:end]
assert "pixargs[10]" in region
for kind in ("MrmRtypeIconImage", "MrmRtypeXBitmapFile"):
    pos = 0
    found = False
    while True:
        try:
            pos = region.index("case " + kind + ":", pos)
        except ValueError:
            break
        try:
            guard = region.index("pixargs_cnt >= sizeof(pixargs) / sizeof(pixargs[0])", pos)
            write = region.index("savepix = &pixargs[pixargs_cnt]", pos)
        except ValueError:
            break
        if guard < write:
            found = True
            break
        pos += 1
    assert found, kind
print("Mrm deferred pixmap storage validates fixed-array capacity")
