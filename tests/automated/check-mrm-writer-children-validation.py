#!/usr/bin/env python3
"""Check Mrm child-list initialization propagates context validation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRInitChildren (URMResourceContextPtr")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
validate = region.find("result = UrmCWR__ValidateContext")
if validate < 0:
    raise SystemExit("children initializer does not capture context validation")
guard = region.find("if ( result != MrmSUCCESS ) return result ;", validate)
if guard < 0 or guard - validate > 180:
    raise SystemExit("children initializer ignores context validation failure")
allocate = region.find("result = UrmCWR__GuaranteeSpace", guard)
if allocate < 0 or allocate < guard:
    raise SystemExit("children allocation is not reached after validation")

print("PASS Mrm children context validation failures propagate")
