#!/usr/bin/env python3
"""Check simple Mrm writer setters propagate context validation failures."""
import os
from pathlib import Path
source=Path(os.environ.get("MOTIF_SOURCE",Path(__file__).parents[2]/"lib"/"Mrm"/"Mrmwcrwr.c"))
text=source.read_text(encoding="utf-8")
for routine in ("UrmCWRSetClass", "UrmCWRSetComment"):
    start=text.index(routine+" ("); end=text.find("\n\n\f",start)
    region=text[start:] if end<0 else text[start:end]
    needle='  result = UrmCWR__ValidateContext (context_id, "'+routine+'") ;\n  if ( result != MrmSUCCESS ) return result ;'
    if needle not in region: raise SystemExit(f"{routine}: context validation failure is ignored")
print("Mrm class/comment validation failures propagate")
