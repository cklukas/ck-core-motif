#!/usr/bin/env python3
"""Check Mrm color and font reader boundaries reject null records."""
import os
from pathlib import Path
source=Path(os.environ.get("MOTIF_SOURCE",Path(__file__).parents[2]/"lib"/"Mrm"/"Mrmwcrw.c"))
text=source.read_text(encoding="utf-8")
start=text.index("\nUrm__CW_CreateArglist ("); end=text.find("\n\n\f",start)
region=text[start:end]
if "if ( colorptr == NULL )" not in region: raise SystemExit("CreateArglist color guard is missing")
color_start=region.index("if (reptype == MrmRtypeColor)")
color_end=region.index("else\n\t\t/*\n\t\t * End Fix for CR 5410", color_start)
color_region=region[color_start:color_end]
pixel_store=color_region.index("val = (long) pix")
failure_guard=color_region.rindex("if ( result != MrmSUCCESS ) continue", 0, pixel_store)
if failure_guard > pixel_store: raise SystemExit("CreateArglist stores an uninitialized color pixel")
start=text.index("\nUrm__CW_FixupValue ("); end=text.find("\n\n\f",start)
region=text[start:end]
if "if ( fontitem == NULL || bufptr == NULL )" not in region: raise SystemExit("font fixup guard is missing")
print("Mrm color conversion failures do not store an uninitialized pixel")
