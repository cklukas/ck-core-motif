#!/usr/bin/env python3
"""Check drop handlers reject missing destination callback data."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
checks = {
    "Container.c": ("dropproc_cs = (XmDropProcCallbackStruct *)cs->destination_data;", "if (dropproc_cs == NULL)"),
    "TextSel.c": ("(XmDropProcCallbackStruct *) ds->destination_data;", "if (cb == NULL)"),
    "TextFSel.c": ("(XmDropProcCallbackStruct *) ds->destination_data;", "if (cb == NULL)"),
}
for name, (assignment, guard) in checks.items():
    source = (root / name).read_text()
    start = source.index(assignment)
    end = source.index("\n}", start)
    body = source[start:end]
    assert guard in body, name
    assert body.index(guard) < body.index("->", body.index(guard) + len(guard)), name

print("drop handlers validate destination callback data")
