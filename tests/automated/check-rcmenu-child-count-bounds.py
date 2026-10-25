#!/usr/bin/env python3
"""Check direct RCMenu child traversals use full-width indices."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RCMenu.c").read_text(
    encoding="utf-8"
)
for signature in (
    "_XmRCArmAndActivate(",
    "ProcessMenuTree(",
    "SearchMenu(",
    "IsInWidgetList(",
):
    start = source.index(signature)
    start = source.index("{", start)
    end = source.find("\n}\n", start)
    if end == -1:
        end = len(source)
    region = source[start:end]
    if "int i" in region or "register int i" in region:
        raise SystemExit(f"signed child index remains in {signature}")
print("RCMenu direct child traversals use full-width Cardinal indices")
