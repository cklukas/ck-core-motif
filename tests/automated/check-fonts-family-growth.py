#!/usr/bin/env python3
"""Check FontSelector family-table growth before Xt allocation."""
from pathlib import Path

source = Path("lib/Xm/FontS.c").read_text()
start = source.index("ProcessXlfdFontData(XmFontSelectorWidget fsw,")
end = source.index("\n/*\tFunction Name:", start)
region = source[start:end]
assert "alloc > INT_MAX - FAMILY_INC" in region
assert "SIZE_MAX / sizeof(*fam)" in region
assert "UINT_MAX / sizeof(*fam)" in region
assert region.index("SIZE_MAX / sizeof(*fam)") < region.index("alloc += FAMILY_INC")
print("FontSelector family-table growth is bounded")
