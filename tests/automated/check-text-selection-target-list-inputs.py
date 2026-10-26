#!/usr/bin/env python3
"""Check target-list callbacks reject missing target-list storage."""

from pathlib import Path


ROOT = Path(__file__).parents[2] / "lib" / "Xm"

for name in ("TextSel.c", "TextFSel.c"):
    source = (ROOT / name).read_text()
    start = source.rindex("HandleTargets(Widget w")
    end = source.find("\nstatic ", start)
    body = source[start:] if end == -1 else source[start:end]
    assert "ds->value == NULL" in body, name
    assert "XtFree((char *)ds->value)" in body, name

print("Text target-list callbacks reject missing target storage")
