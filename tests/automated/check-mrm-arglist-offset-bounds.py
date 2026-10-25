#!/usr/bin/env python3
"""Check both Mrm reader arglist paths bound descriptor offsets."""
from pathlib import Path
import re

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
guards = re.findall(
    r"widgetrec->arglist_offs\s*<\s*RGMWidgetRecordHdrSize"
    r"[\s\S]{0,180}?widgetrec->arglist_offs\s*>=\s*widgetrec->size",
    text,
)
if len(guards) != 2:
    raise SystemExit("both reader arglist paths lack offset bounds")
print("PASS Mrm reader arglist offsets are bounded")
