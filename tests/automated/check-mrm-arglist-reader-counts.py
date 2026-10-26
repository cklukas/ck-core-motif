#!/usr/bin/env python3
"""Check both Mrm reader arglist paths bound descriptor counts."""
import os
from pathlib import Path
source=Path(os.environ.get("MOTIF_SOURCE",Path(__file__).parents[2]/"lib"/"Mrm"/"Mrmwcrw.c"))
text=source.read_text(encoding="utf-8")
needle='''      if (argdesc->count < 0 || argdesc->extra < 0 ||\n          argdesc->count > RGMListSizeMax ||\n          argdesc->extra > RGMListSizeMax ||\n          (size_t)argdesc->count >\n              (widgetrec->size - widgetrec->arglist_offs -\n               (sizeof(RGMArgListDesc) - sizeof(RGMArgument))) /\n              sizeof(RGMArgument))\n        return MrmFAILURE;'''
if text.count(needle) != 2: raise SystemExit("both reader arglist paths lack count limits")
print("Mrm reader arglist counts are bounded")
