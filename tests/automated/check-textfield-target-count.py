#!/usr/bin/env python3
"""Check TextField target callbacks bound external target counts."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("HandleTargets(Widget w")
end = source.index("\nstatic void\nHandleDrop", start)
body = source[start:end]
assert "unsigned long i;" in body
assert "ds->transfer_id == NULL" in body
assert "ds->length > (unsigned long)INT_MAX" in body
assert "for (i = 0; i < ds->length; i++, atom_ptr++)" in body

print("TextField target counts are bounded")
