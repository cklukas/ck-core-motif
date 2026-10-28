#!/usr/bin/env python3
"""Check TextField global selection state is distinct from callback locals."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text(
    encoding="utf-8"
)
assert "static _XmInsertSelect insert_select_state;" in source
assert "&insert_select_state, ds->time" in source
assert "static _XmInsertSelect insert_select;" not in source
assert "static _XmTextPrimSelect *primary_select_state;" in source
assert "static _XmTextPrimSelect *prim_select;" not in source
print("PASS TextField global selection state has an unambiguous name")
