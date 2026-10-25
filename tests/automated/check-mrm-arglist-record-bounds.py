#!/usr/bin/env python3
"""Check both Mrm widget-instance paths bound serialized argument records."""
import os
from pathlib import Path

source = Path(os.environ.get("MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"))
text = source.read_text(encoding="utf-8")
required = (
    "widgetrec->size - widgetrec->arglist_offs <",
    "sizeof(RGMArgListDesc) - sizeof(RGMArgument)",
    "argdesc->count < 0 || argdesc->extra < 0",
    "(size_t)argdesc->count >",
    "sizeof(RGMArgument))",
)
for routine in ("UrmCreateWidgetInstance (", "UrmSetWidgetInstance ("):
    start = text.index("\n" + routine)
    end = text.find("\n\n\f", start)
    region = text[start:end]
    for needle in required:
        if needle not in region:
            raise SystemExit(f"{routine}: missing argument-record bound: {needle}")
print("Mrm create and set paths bound serialized argument records")
