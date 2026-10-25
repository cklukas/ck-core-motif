#!/usr/bin/env python3
"""Check null-safe inputs for the Mrm icon loader."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(encoding="utf-8")
start = source.index("Urm__CW_LoadIconImage (RGMIconImagePtr")
end = source.find("\nCardinal ", start + 1)
body = source[start:] if end < 0 else source[start:end]
needle = "if ( iconptr == NULL || bufptr == NULL || ctxlist == NULL )"
assert needle in body
assert "return MrmFAILURE;" in body
assert body.index(needle) < body.index("iconptr->validation")
assert body.index(needle) < body.index("(char *)bufptr")
print("Mrm icon loader validates its pointer inputs")
