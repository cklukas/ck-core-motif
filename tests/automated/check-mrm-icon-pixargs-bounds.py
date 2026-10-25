#!/usr/bin/env python3
"""Check deferred Mrm icon storage stays within its fixed local array."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_CreateArglist (")
end = text.find("\n\n\f", start)
region = text[start:end]
needle = (
    "case MrmRtypeIconImage:\n"
    "\t  if (pixargs_cnt >= sizeof(pixargs) / sizeof(pixargs[0]))"
)
if needle not in region:
    raise SystemExit("icon deferred-storage capacity is not checked")
print("PASS Mrm deferred icon storage is bounded")
