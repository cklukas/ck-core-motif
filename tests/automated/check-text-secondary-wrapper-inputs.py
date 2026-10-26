#!/usr/bin/env python3
"""Check Text secondary-selection wrappers validate callback inputs."""

from pathlib import Path


for name, marker in (
    ("TextSel.c", "TextSecondaryWrapper(Widget w"),
    ("TextFSel.c", "TextFieldSecondaryWrapper(Widget w"),
):
    source = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text()
    start = source.rindex(marker)
    end = source.find("\n}\n", start)
    body = source[start:] if end == -1 else source[start:end]
    assert "w == NULL" in body, name
    assert "closure == NULL" in body, name
    assert "ds == NULL" in body, name
    assert "ds->transfer_id == NULL" in body, name

print("Text secondary-selection wrappers validate inputs")
