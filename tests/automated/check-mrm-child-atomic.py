#!/usr/bin/env python3
"""Ensure Mrm child descriptors are written after key validation."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c").read_text(
    encoding="utf-8"
)
start = text.index("UrmCWRSetChild (")
body = text[start:text.index("\n}\n", start)]
switch_pos = body.index("switch ( key_type )")
append_pos = body.index("UrmCWR__AppendString", switch_pos)
write_pos = body.index("childptr->manage = manage", append_pos)
assert switch_pos < append_pos < write_pos
assert "childptr->type = URMrIndex" in body
assert "childptr->type = URMrRID" in body
print("Mrm child references commit after key validation")
