#!/usr/bin/env python3
"""Check Mrm callback-argument setup validates context, output and binding."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRSetArgCallback (URMResourceContextPtr")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
output = region.find("if ( cb_offs_return == NULL )")
if output < 0:
    raise SystemExit("callback-argument setter lacks output-pointer validation")
validate = region.find("result = UrmCWR__ValidateContext", output)
if validate < 0:
    raise SystemExit("callback-argument setter does not capture context validation")
first_guard = region.find("if ( result != MrmSUCCESS ) return result ;", validate)
if first_guard < 0 or first_guard - validate > 180:
    raise SystemExit("callback-argument setter ignores context validation failure")
first_bind = region.find("result = UrmCWR__BindArgPtrs", first_guard)
second_bind = region.find("result = UrmCWR__BindArgPtrs", first_bind + 1)
if first_bind < 0 or second_bind < 0:
    raise SystemExit("callback-argument setter does not rebind after growth")
second_guard = region.find("if ( result != MrmSUCCESS ) return result ;", second_bind)
if second_guard < 0 or second_guard - second_bind > 180:
    raise SystemExit("callback-argument setter ignores post-growth binding failure")

print("Mrm callback-argument validation failures propagate")
