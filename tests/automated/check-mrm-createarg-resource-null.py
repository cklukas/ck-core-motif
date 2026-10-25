#!/usr/bin/env python3
"""Check argument-list resource values reject null descriptors."""
import os
from pathlib import Path
source=Path(os.environ.get("MOTIF_SOURCE",Path(__file__).parents[2]/"lib"/"Mrm"/"Mrmwcrw.c"))
text=source.read_text(encoding="utf-8")
start=text.index("\nUrm__CW_CreateArglist ("); end=text.find("\n\n\f",start)
region=text[start:end]
needle='''\t  resptr = (RGMResourceDescPtr) val ;\n\t  if ( resptr == NULL )\n\t    continue ;'''
if needle not in region: raise SystemExit("resource descriptor null guard is missing")
print("Mrm argument-list resource values guard null descriptors")
