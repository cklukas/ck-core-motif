#!/usr/bin/env python3
"""Check both Mrm reader arglist paths bound descriptor offsets."""
import os
from pathlib import Path
source=Path(os.environ.get("MOTIF_SOURCE",Path(__file__).parents[2]/"lib"/"Mrm"/"Mrmwcrw.c"))
text=source.read_text(encoding="utf-8")
needle='''      if (widgetrec->arglist_offs < RGMWidgetRecordHdrSize ||\n          widgetrec->arglist_offs >= widgetrec->size ||\n          widgetrec->size - widgetrec->arglist_offs <\n              sizeof(RGMArgListDesc) - sizeof(RGMArgument))\n        return MrmFAILURE;'''
if text.count(needle) != 2: raise SystemExit("both reader arglist paths lack offset bounds")
print("Mrm reader arglist offsets are bounded")
