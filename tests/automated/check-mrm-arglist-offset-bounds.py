#!/usr/bin/env python3
"""Check both Mrm reader arglist paths bound descriptor offsets."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
needle = ("if (widgetrec->arglist_offs < RGMWidgetRecordHdrSize ||\n"
          "\t  widgetrec->arglist_offs >= widgetrec->size)\n"
          "\treturn MrmFAILURE;")
if text.count(needle) != 2:
    raise SystemExit("both reader arglist paths lack offset bounds")
print("PASS Mrm reader arglist offsets are bounded")
