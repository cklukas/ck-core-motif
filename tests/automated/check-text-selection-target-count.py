#!/usr/bin/env python3
"""Check external Text target counts are bounded before iteration."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text()
start = source.index("HandleTargets(")
end = source.index("\nstatic void\nDoStuff", start)
body = source[start:end]
assert "unsigned long i;" in body
assert "ds->length > (unsigned long)INT_MAX" in body
assert "for (i = 0; i < ds->length; i++, atom_ptr++)" in body

print("Text selection target counts are bounded")
