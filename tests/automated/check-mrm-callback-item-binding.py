#!/usr/bin/env python3
"""Check callback item setters propagate descriptor binding failures."""
import re
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
for routine, expected in (("UrmCWRSetCallbackItem", 6),
                         ("UrmCWRSetCallbackItemRes", 3)):
    start = text.index(routine + " (")
    end = text.find("\n\n\f", start)
    region = text[start:] if end < 0 else text[start:end]
    binds = re.findall(r"(?m)^\s*(result = )?UrmCWR__BindCallbackPtrs", region)
    if len(binds) != expected:
        raise SystemExit(f"{routine}: expected {expected} callback bindings, found {len(binds)}")
    if any(not assigned for assigned in binds):
        raise SystemExit(f"{routine}: ignored callback binding")
    if region.count("result = UrmCWR__ValidateContext") != 1:
        raise SystemExit(f"{routine}: context validation is not captured")
    if region.count("if ( result != MrmSUCCESS ) return result ;") < expected + 1:
        raise SystemExit(f"{routine}: missing failure guards")

print("PASS Mrm callback item binding failures propagate")
