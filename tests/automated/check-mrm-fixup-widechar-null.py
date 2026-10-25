#!/usr/bin/env python3
"""Check wide-character fixup rejects a null serialized entry."""
import os
from pathlib import Path
source=Path(os.environ.get("MOTIF_SOURCE",Path(__file__).parents[2]/"lib"/"Mrm"/"Mrmwcrw.c"))
text=source.read_text(encoding="utf-8")
start=text.index("\nUrm__CW_FixupValue ("); end=text.find("\n\n\f",start)
region=text[start:end]
needle='''      wcharentry = (RGMWCharEntryPtr)val;\n      if ( wcharentry == NULL )\n\treturn MrmFAILURE ;'''
if needle not in region: raise SystemExit("wide-character fixup null guard is missing")
print("Mrm wide-character fixup guards null entries")
