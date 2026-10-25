#!/usr/bin/env python3
"""Check Mrm child setup propagates context validation failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRSetChild (URMResourceContextPtr")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
validate = region.find("result = UrmCWR__ValidateContext")
if validate < 0:
    raise SystemExit("child setter does not capture context validation")
guard = region.find("if ( result != MrmSUCCESS ) return result ;", validate)
if guard < 0 or guard - validate > 180:
    raise SystemExit("child setter ignores context validation failure")
record = region.find("widgetrec = (RGMWidgetRecordPtr) UrmRCBuffer", guard)
if record < 0 or record < guard:
    raise SystemExit("child setter does not access the record after validation")

print("Mrm child context validation failures propagate")
