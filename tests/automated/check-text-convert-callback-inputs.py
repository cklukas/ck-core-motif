#!/usr/bin/env python3
"""Check Text conversion callback wrappers validate widget and state."""

from pathlib import Path


for name, marker in (
    ("TextSel.c", "TextConvertCallback(Widget"),
    ("TextFSel.c", "TextFieldConvertCallback(Widget"),
):
    source = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text()
    start = source.rindex(marker)
    body = source[start:source.index("\nstatic ", start)]
    assert "if (w == NULL || cs == NULL)" in body, name
    assert "XmeGetEncodingAtom(w)" in body, name
    assert body.index("if (w == NULL || cs == NULL)") < body.index("XmeGetEncodingAtom(w)"), name

print("Text conversion callbacks validate widget and callback state")
