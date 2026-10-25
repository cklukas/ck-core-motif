#!/usr/bin/env python3
"""Check null-safe inputs for the shared Mrm icon converters."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmicon.c").read_text(encoding="utf-8")
for signature in ("UrmCreatePixmap (RGMIconImagePtr", "UrmCreateBitmap (RGMIconImagePtr"):
    start = source.index(signature)
    end = source.find("\nCardinal ", start + 1)
    body = source[start:] if end < 0 else source[start:end]
    assert "if ( icon == NULL || pixmap == NULL )" in body
    assert "return MrmFAILURE;" in body
    assert "*pixmap = 0;" in body
    assert body.index("if ( icon == NULL || pixmap == NULL )") < body.index("icon->")
print("Mrm icon converters validate and initialize their inputs")
