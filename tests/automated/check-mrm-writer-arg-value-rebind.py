#!/usr/bin/env python3
"""Check Mrm arg-value post-growth rebinds propagate failures."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRSetArgValue (URMResourceContextPtr")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
needle = "      result = UrmCWR__BindArgPtrs"
count = region.count(needle)
if count != 6:
    raise SystemExit(f"expected six post-growth arg-value binds, found {count}")
checks = region.count("      if ( result != MrmSUCCESS ) return result ;")
lines = region.splitlines()
bind_checks = 0
for index, line in enumerate(lines):
    if line == needle:
        if not any("if ( result != MrmSUCCESS ) return result ;" in item
                   for item in lines[index:index + 4]):
            raise SystemExit("an arg-value rebind lacks an immediate failure check")
        bind_checks += 1
if bind_checks != 6:
    raise SystemExit(f"expected six post-growth arg-value bind checks, found {bind_checks}")

print("Mrm arg-value post-growth rebind failures propagate")
