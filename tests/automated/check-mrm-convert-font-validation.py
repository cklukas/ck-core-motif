#!/usr/bin/env python3
"""Check Mrm font conversion validates font records before strlen/use."""
import os
from pathlib import Path
source=Path(os.environ.get("MOTIF_SOURCE",Path(__file__).parents[2]/"lib"/"Mrm"/"Mrmwcrw.c"))
text=source.read_text(encoding="utf-8")
start=text.index("\nUrm__CW_ConvertValue ("); end=text.find("\n\n\f",start)
region=text[start:] if end<0 else text[start:end]
if "fontptr == NULL || fontptr->font.font == NULL" not in region: raise SystemExit("font-item guard missing")
if "fontlist == NULL || fontlist->validation != URMFontListValid" not in region: raise SystemExit("font-list metadata guard missing")
if "if ( fontstg == NULL )" not in region: raise SystemExit("font-string guard missing")
print("Mrm font conversion validates font records")
