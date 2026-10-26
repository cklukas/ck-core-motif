#!/usr/bin/env python3
"""Check Text and TextField selection-loss callbacks validate inputs."""

from pathlib import Path


for name, marker in (
    ("TextSel.c", "_XmTextLoseSelection(\n"),
    ("TextFSel.c", "_XmTextFieldLoseSelection(\n"),
):
    source = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text()
    start = source.index(marker)
    end = source.index("\n}\n", start)
    body = source[start:end]
    assert "selection == NULL" in body, name
    assert "*selection == None" in body, name
    assert "return;" in body, name

print("Text selection-loss callbacks validate inputs")
