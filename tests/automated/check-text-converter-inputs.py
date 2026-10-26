#!/usr/bin/env python3
"""Check Text and TextField converters validate pointers before X11 access."""

from pathlib import Path


for name, marker in (
    ("TextSel.c", "_XmTextConvert(\n"),
    ("TextFSel.c", "_XmTextFieldConvert(\n"),
):
    source = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text()
    start = source.index(marker)
    end = source.index("\n}\n", start)
    body = source[start:end]
    guard = body.index("selection == NULL")
    assert "type == NULL" in body[guard:guard + 180], name
    assert "value == NULL" in body[guard:guard + 180], name
    assert "return False;" in body[guard:guard + 220], name
    if name == "TextFSel.c":
        assert body.index("XmeGetEncodingAtom(w)") > guard, name

print("Text converters validate callback pointers before display access")
