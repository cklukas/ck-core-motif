#!/usr/bin/env python3
"""Check I18List column-to-pixel accumulation cannot wrap short."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "long long value = 0;" in source
assert "return bounded_short(value);" in source
assert "short value=0;" not in source
print("PASS I18List column pixel offsets stay bounded")
