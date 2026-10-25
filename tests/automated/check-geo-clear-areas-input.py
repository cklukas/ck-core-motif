#!/usr/bin/env python3
"""Check that _XmGeoClearRectObjAreas validates geometry inputs."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "GeoUtils.c"
text = source.read_text(encoding="utf-8")
for needle in ("if (r == NULL || old == NULL)", "if (parent == NULL)"):
    if needle not in text:
        raise SystemExit(f"clear-area input guard is missing: {needle}")

print("clear-area input guards present")
