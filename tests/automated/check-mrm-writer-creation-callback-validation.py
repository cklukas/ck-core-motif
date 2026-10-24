#!/usr/bin/env python3
"""Check Mrm creation-callback setup validates context and output storage."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRSetCreationCallback (")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
output = region.find("if ( cb_offs_return == NULL )")
if output < 0:
    raise SystemExit("creation-callback setup lacks output-pointer validation")
validate = region.find("result = UrmCWR__ValidateContext", output)
if validate < 0:
    raise SystemExit("creation-callback setup does not capture context validation")
guard = region.find("if ( result != MrmSUCCESS ) return result ;", validate)
if guard < 0 or guard - validate > 180:
    raise SystemExit("creation-callback setup ignores context validation failure")
write = region.find("*cb_offs_return = offset", guard)
if write < 0:
    raise SystemExit("creation-callback setup no longer returns its descriptor offset")

print("Mrm creation-callback validation failures propagate")
