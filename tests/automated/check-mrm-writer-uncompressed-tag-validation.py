#!/usr/bin/env python3
"""Check Mrm uncompressed-tag setup propagates validation failures."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRSetUncompressedArgTag (")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
validate = region.find("result = UrmCWR__ValidateContext")
if validate < 0:
    raise SystemExit("uncompressed-tag setter does not capture context validation")
first_guard = region.find("if ( result != MrmSUCCESS ) return result ;", validate)
if first_guard < 0 or first_guard - validate > 180:
    raise SystemExit("uncompressed-tag setter ignores context validation failure")
first_bind = region.find("result = UrmCWR__BindArgPtrs", first_guard)
if first_bind < 0:
    raise SystemExit("uncompressed-tag setter does not capture initial binding")
second_bind = region.find("result = UrmCWR__BindArgPtrs", first_bind + 1)
if second_bind < 0:
    raise SystemExit("uncompressed-tag setter does not capture post-append binding")
second_guard = region.find("if ( result != MrmSUCCESS ) return result ;", second_bind)
if second_guard < 0 or second_guard - second_bind > 180:
    raise SystemExit("uncompressed-tag setter ignores post-append binding failure")

print("Mrm uncompressed-tag validation failures propagate")
