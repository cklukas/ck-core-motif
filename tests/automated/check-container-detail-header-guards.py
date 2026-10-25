#!/usr/bin/env python3
"""Check Container clears stale detail-header cache on allocation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "Container.c"
text = source.read_text()
start = text.index("static\tvoid\nGetDetailHeader")
end = text.index("static\tvoid\nGetDetailHeaderCount", start)
block = text[start:end]

if "cache_detail_heading = NULL" not in block:
    raise SystemExit("detail-header cache is not cleared before replacement")
if "cache_detail_heading == NULL" not in block:
    raise SystemExit("detail-header allocation failure is not guarded")
if "UINT_MAX / sizeof(XmString)" not in block:
    raise SystemExit("detail-header allocation lacks Xt Cardinal-width guard")
if "XtMalloc((Cardinal)(sizeof(XmString) *" not in block:
    raise SystemExit("detail-header allocation lacks explicit Xt width cast")

print("Container detail-header cache allocation is guarded")
