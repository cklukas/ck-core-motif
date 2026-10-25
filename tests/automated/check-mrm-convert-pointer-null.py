#!/usr/bin/env python3
"""Check null pointer-backed values before Mrm conversion helpers."""
import os
from pathlib import Path
source=Path(os.environ.get("MOTIF_SOURCE",Path(__file__).parents[2]/"lib"/"Mrm"/"Mrmwcrw.c"))
text=source.read_text(encoding="utf-8")
start=text.index("\nUrm__CW_ConvertValue ("); end=text.find("\n\n\f",start)
region=text[start:] if end<0 else text[start:end]
region=region[region.index("switch ( reptype )"):]
for case in ("MrmRtypeCString", "MrmRtypeAddrName", "MrmRtypeIconImage", "MrmRtypeXBitmapFile"):
    marker=f"\n    case {case}:"
    pos=region.index(marker)
    tail=region[pos:region.find("\n    case ",pos+len(marker)) if region.find("\n    case ",pos+len(marker)) >= 0 else len(region)]
    if "if ( *val == 0 )" not in tail: raise SystemExit(f"{case} lacks null-value guard")
print("Mrm pointer-backed conversions guard null values")
