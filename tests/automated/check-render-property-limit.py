#!/usr/bin/env python3
"""Check bounds before indexing the render property-name table."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c"
text = source.read_text(encoding="utf-8")

needle = "if (j >= (int)(sizeof(items) / sizeof(items[0])))"
if needle not in text:
    raise SystemExit("render property-name table bound check is missing")
if "goto error;" not in text:
    raise SystemExit("render property overflow does not use cleanup")

print("render property-name indexing is bounded")
