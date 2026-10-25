#!/usr/bin/env python3
"""Check Mwm feedback includes the limit definitions used by its guard."""

from pathlib import Path


source = Path(__file__).parents[2] / "clients" / "mwm" / "WmFeedback.c"
text = source.read_text(encoding="utf-8")
assert "#include <limits.h>" in text
assert "length >= UINT_MAX" in text
print("WmFeedback size-limit header present")
