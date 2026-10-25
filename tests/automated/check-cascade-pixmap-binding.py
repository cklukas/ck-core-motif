#!/usr/bin/env python3
"""Check CascadeButton pixmap glyphs accept fetched metadata."""
from pathlib import Path

root = Path(__file__).resolve().parents[2] / "lib" / "Xm"
for filename in ("CascadeB.c", "CascadeBG.c"):
    text = (root / filename).read_text(encoding="utf-8")
    start = text.index("\nDrawCascade(\n") + 1
    end = text.index("\n}\n", start) + 3
    body = text[start:end]
    if "int depth = 0;" not in body:
        raise SystemExit(f"{filename} cascade depth is not initialized")
    if "if (depth <= 0)" not in body:
        raise SystemExit(f"{filename} does not reject unresolved pixmap depth")
    if "if (!XmeGetPixmapData" in body:
        raise SystemExit(f"{filename} still treats cache status as failure")
print("CascadeButton pixmap glyphs accept fetched metadata")
