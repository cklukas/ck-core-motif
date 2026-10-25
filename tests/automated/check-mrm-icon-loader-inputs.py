#!/usr/bin/env python3
"""Check null-safe inputs for the Mrm icon loader."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(encoding="utf-8")
start = source.index("Urm__CW_LoadIconImage (RGMIconImagePtr")
end = source.find("\nCardinal ", start + 1)
body = source[start:] if end < 0 else source[start:end]
needle = "if ( iconptr == NULL || bufptr == NULL || ctxlist == NULL )"
guard = body.index(needle)
if "return MrmFAILURE;" not in body[guard:guard + 100]:
    raise SystemExit("icon loader does not fail invalid pointer inputs")
if guard > body.index("iconptr->validation") or guard > body.index("(char *)bufptr"):
    raise SystemExit("icon loader dereferences input before validation")
print("PASS Mrm icon loader validates its pointer inputs")
