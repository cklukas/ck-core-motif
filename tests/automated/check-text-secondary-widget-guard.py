#!/usr/bin/env python3
"""Check secondary Text callbacks validate widgets before atom queries."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text()
start = source.index("HandleInsertTargets(")
end = source.index("\nstatic void\nHandleDrop", start)
body = source[start:end]
assert "Atom CS_OF_ENCODING;" in body
assert body.index("w == NULL || closure == NULL") < body.index("CS_OF_ENCODING = XmeGetEncodingAtom(w);")

print("Text secondary callbacks guard widgets before atom queries")
