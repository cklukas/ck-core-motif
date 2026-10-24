#!/usr/bin/env python3
"""Check Mrm resource callback routine input is validated."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRSetCallbackItemRes (")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
check = region.find("if ( routine == NULL || strlen(routine) <= 0 )")
if check < 0:
    raise SystemExit("resource callback routine lacks null-input validation")
strlen = region.find("strlen(routine)", check)
if strlen < check:
    raise SystemExit("resource callback routine is not checked before string access")

print("Mrm writer resource callback routine input is validated")
