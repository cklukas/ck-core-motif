#!/usr/bin/env python3
"""Check null-safe inputs for the shared Mrm icon converters."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmicon.c").read_text(encoding="utf-8")
for signature in ("UrmCreatePixmap (RGMIconImagePtr", "UrmCreateBitmap (RGMIconImagePtr"):
    start = source.index(signature)
    end = source.find("\nCardinal ", start + 1)
    body = source[start:] if end < 0 else source[start:end]
    guard = body.index("if ( icon == NULL || pixmap == NULL )")
    if "return MrmFAILURE;" not in body[guard:guard + 80] or "*pixmap = 0;" not in body[guard:guard + 100]:
        raise SystemExit(f"{signature} does not initialize failure-safe outputs")
    if guard > body.index("icon->"):
        raise SystemExit(f"{signature} dereferences icon before validation")
print("PASS Mrm icon converters validate and initialize their inputs")
