#!/usr/bin/env python3
"""Check Text primary-selection release paths prevent refcount underflow."""

from pathlib import Path


for name in ("TextSel.c", "TextFSel.c"):
    source = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text()
    start = source.index("SetPrimarySelection(Widget w")
    end = source.index("static void", start + 20)
    set_body = source[start:end]
    start = source.index("CleanPrimarySelection(Widget w")
    end = source.index("static void", start + 20)
    clean_body = source[start:end]
    for body in (set_body, clean_body):
        assert "prim_select->ref_count <= 0" in body, name
        assert "--prim_select->ref_count == 0" in body, name

print("Text primary-selection release paths prevent refcount underflow")
