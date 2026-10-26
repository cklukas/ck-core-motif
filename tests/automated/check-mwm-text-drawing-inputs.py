#!/usr/bin/env python3
"""Check null-input handling in MWM text drawing helpers."""

from pathlib import Path


source = Path("clients/mwm/WmGraphics.c").read_text(encoding="utf-8")

for marker, first_use in (
    ("void DrawStringInBox", "XTextWidth"),
    ("void WmDrawString", "ACTIVE_PSD->cleanText"),
):
    start = source.index(marker)
    end = source.index("END OF FUNCTION", start)
    body = source[start:end]
    assert "== NULL" in body
    assert "return;" in body
    assert body.index("== NULL") < body.index(first_use)

print("PASS MWM text drawing rejects incomplete inputs")
