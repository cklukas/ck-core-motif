#!/usr/bin/env python3
"""Check compact-height guards for MWM system and minimize glyphs."""

from pathlib import Path


source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
checks = (
    ("void BevelSystemButton", "if (height < 5)", "height - 4"),
    ("void BevelMinimizeButton", "if (height < 7)", "height-6"),
)

for marker, guard, arithmetic in checks:
    start = source.index(marker)
    end = source.index("} /* END OF FUNCTION", start)
    body = source[start:end]
    assert guard in body
    assert body.index(guard) < body.index(arithmetic)
    assert body.index(guard) < body.index("BevelRectangle")

print("PASS MWM compact gadget geometry rejects undersized heights")
