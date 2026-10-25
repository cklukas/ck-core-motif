#!/usr/bin/env python3
"""Check null pointer-backed values before Mrm conversion helpers."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_ConvertValue (")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
region = region[region.index("switch ( reptype )"):]
for case in ("MrmRtypeCString", "MrmRtypeAddrName", "MrmRtypeIconImage",
             "MrmRtypeXBitmapFile"):
    marker = "\n    case " + case + ":"
    pos = region.index(marker)
    next_case = region.find("\n    case ", pos + len(marker))
    tail = region[pos:next_case if next_case >= 0 else len(region)]
    if "if ( *val == 0 )" not in tail:
        raise SystemExit(f"{case} lacks null-value guard")
print("PASS Mrm pointer-backed conversions guard null values")
