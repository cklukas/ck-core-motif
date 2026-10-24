#!/usr/bin/env python3
"""Check Mrm arglist initialization propagates context validation failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRInitArglist (")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
validate = region.find("result = UrmCWR__ValidateContext")
if validate < 0:
    raise SystemExit("arglist initialization does not capture context validation")
guard = region.find("if ( result != MrmSUCCESS ) return result ;", validate)
if guard < 0 or guard - validate > 180:
    raise SystemExit("arglist initialization ignores context validation failure")
allocate = region.find("result = UrmCWR__GuaranteeSpace", guard)
if allocate < 0 or allocate < guard:
    raise SystemExit("arglist allocation is not reached after validation")

print("Mrm arglist context validation failures propagate")
