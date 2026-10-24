#!/usr/bin/env python3
"""Check Mrm compressed-tag setup propagates validation failures."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRSetCompressedArgTag (")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
validate = region.find("result = UrmCWR__ValidateContext")
if validate < 0:
    raise SystemExit("compressed-tag setter does not capture context validation")
guard = region.find("if ( result != MrmSUCCESS ) return result ;", validate)
if guard < 0 or guard - validate > 180:
    raise SystemExit("compressed-tag setter ignores context validation failure")
bind = region.find("result = UrmCWR__BindArgPtrs", guard)
if bind < 0:
    raise SystemExit("compressed-tag setter does not capture descriptor binding")
bind_guard = region.find("if ( result != MrmSUCCESS ) return result ;", bind)
if bind_guard < 0 or bind_guard - bind > 180:
    raise SystemExit("compressed-tag setter ignores descriptor binding failure")
write = region.find("argptr->tag_code = tag", bind_guard)
if write < 0 or write < bind_guard:
    raise SystemExit("compressed-tag setter writes before validation")

print("Mrm compressed-tag validation failures propagate")
