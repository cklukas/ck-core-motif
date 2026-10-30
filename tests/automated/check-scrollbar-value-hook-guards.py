#!/usr/bin/env python3
"""Check ScrollBar value and default callbacks validate inputs."""

from pathlib import Path


source = Path("lib/Xm/ScrollBar.c").read_text()
for signature in (
    "ExportScrollBarValue(",
    "ImportScrollBarValue(",
    "ProcessingDirectionDefault(",
):
    start = source.rindex(signature)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "value == NULL" in body, signature

for signature in ("ExportScrollBarValue(", "ImportScrollBarValue("):
    start = source.rindex(signature)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "xmScrollBarWidgetClass" in body, signature

print("ScrollBar value and default callbacks validate inputs")
