#!/usr/bin/env python3
"""Check Text insert-target callbacks validate transfer inputs."""

from pathlib import Path


for name in ("TextSel.c", "TextFSel.c"):
    source = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text()
    start = source.rindex("HandleInsertTargets(")
    end = source.find("\n/*", start)
    body = source[start:] if end == -1 else source[start:end]
    for token in ("w == NULL", "closure == NULL", "length == NULL",
                  "value == NULL", "tid == NULL"):
        assert token in body, (name, token)

print("Text insert-target callbacks validate transfer inputs")
