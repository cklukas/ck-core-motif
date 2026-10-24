#!/usr/bin/env python3
"""Check Mrm literal contexts are released if ownership-list retention fails."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("Cardinal \nUrm__CW_ReadLiteral (")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
append = region.index("result = UrmPlistAppendPointer (ctxlist")
check = region.find("if ( result != MrmSUCCESS )", append)
if check < 0 or check - append > 180:
    raise SystemExit("literal context retention failure is unchecked")
free = region.find("UrmFreeResourceContext (context_id)", check)
if free < 0 or free - check > 180:
    raise SystemExit("literal context is not released after retention failure")
error = region.find("_MrmMMsg_0001", check)
if error < 0 or error - check > 240:
    raise SystemExit("literal context retention failure lacks an error result")

print("Mrm literal context retention failure releases the context")
