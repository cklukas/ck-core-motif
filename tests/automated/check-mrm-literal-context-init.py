#!/usr/bin/env python3
"""Check Mrm literal loading propagates context allocation failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("Cardinal \nUrm__CW_ReadLiteral (")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
call = region.index("result = UrmGetResourceContext")
check = region.find("if ( result != MrmSUCCESS || context_id == NULL )", call)
if check < 0 or check - call > 220:
    raise SystemExit("literal context allocation failure is unchecked")
error = region.find("_MrmMMsg_0001", check)
if error < 0 or error - check > 260:
    raise SystemExit("literal context allocation failure lacks an error result")
use = region.find("UrmRCType (context_id)", call)
if use < 0 or check > use:
    raise SystemExit("literal context is used before allocation failure is checked")

print("Mrm literal context allocation failure propagates")
