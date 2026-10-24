#!/usr/bin/env python3
"""Check Mrm widget-reference insertion validates context and name inputs."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwrefs.c"
text = source.read_text(encoding="utf-8")
start = text.index("Urm__CW_AddWRef (URMResourceContextPtr")
init = text.index("refdsc = (URMWRefStructPtr) UrmRCBuffer", start)
region = text[start:init]
check = region.index("if (!UrmRCValid(wref_id) || w_name == NULL)")
deref = len(region)
if "return MrmFAILURE;" not in region[check:deref]:
    raise SystemExit("Mrm widget-reference null-input failure is missing")

print("Mrm widget-reference insertion validates inputs")
