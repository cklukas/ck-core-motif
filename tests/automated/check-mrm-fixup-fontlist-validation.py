#!/usr/bin/env python3
"""Check font-list fixup validates buffers and item counts."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_FixupValue (")
end = text.find("\n\n\f", start)
region = text[start:end]
if "if ( bufptr == NULL || val == 0 )" not in region:
    raise SystemExit("font-list buffer guard missing")
if "if ( oldlist->count > RGMListSizeMax )" not in region:
    raise SystemExit("old font-list count guard missing")
if "if ( fontlist->count > RGMListSizeMax )" not in region:
    raise SystemExit("font-list count guard missing")
print("PASS Mrm font-list fixup validates buffers and counts")
