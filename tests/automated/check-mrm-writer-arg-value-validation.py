#!/usr/bin/env python3
"""Check Mrm arg-value setter propagates context and binding failures."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRSetArgValue (")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
validate = region.find("result = UrmCWR__ValidateContext")
if validate < 0:
    raise SystemExit("arg-value setter does not capture context validation")
guard = region.find("if ( result != MrmSUCCESS ) return result ;", validate)
if guard < 0 or guard - validate > 180:
    raise SystemExit("arg-value setter ignores context validation failure")
bind = region.find("result = UrmCWR__BindArgPtrs", guard)
if bind < 0:
    raise SystemExit("arg-value setter does not capture argument binding")
bind_guard = region.find("if ( result != MrmSUCCESS ) return result ;", bind)
if bind_guard < 0 or bind_guard - bind > 180:
    raise SystemExit("arg-value setter ignores argument binding failure")
write = region.find("argptr->arg_val.rep_type = type", bind_guard)
if write < 0 or write < bind_guard:
    raise SystemExit("arg-value setter writes before validation")

print("Mrm arg-value validation failures propagate")
