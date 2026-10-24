#!/usr/bin/env python3
"""Check Mrm extra-args setter propagates validation failures."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRSetExtraArgs (URMResourceContextPtr")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
validate = region.find("result = UrmCWR__ValidateContext")
if validate < 0:
    raise SystemExit("extra-args setter does not capture context validation")
validate_guard = region.find("if ( result != MrmSUCCESS ) return result ;", validate)
if validate_guard < 0 or validate_guard - validate > 180:
    raise SystemExit("extra-args setter ignores context validation failure")
bind = region.find("result = UrmCWR__BindArgPtrs", validate_guard)
if bind < 0:
    raise SystemExit("extra-args setter does not capture descriptor binding")
bind_guard = region.find("if ( result != MrmSUCCESS ) return result ;", bind)
if bind_guard < 0 or bind_guard - bind > 180:
    raise SystemExit("extra-args setter ignores descriptor binding failure")

print("Mrm extra-args validation failures propagate")
