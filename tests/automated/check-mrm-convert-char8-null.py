#!/usr/bin/env python3
"""Check null Char8 values before toolkit string conversions."""
import os
from pathlib import Path
source=Path(os.environ.get("MOTIF_SOURCE",Path(__file__).parents[2]/"lib"/"Mrm"/"Mrmwcrw.c"))
text=source.read_text(encoding="utf-8")
start=text.index("\nUrm__CW_ConvertValue ("); end=text.find("\n\n\f",start)
region=text[start:] if end<0 else text[start:end]
needle='''    case MrmRtypeChar8:\n      {\n        if ( *val == 0 )\n          return MrmFAILURE ;\n        switch ( cvttype )'''
if needle not in region: raise SystemExit("Char8 conversion lacks null-source guard")
print("Mrm Char8 conversion guards null sources")
