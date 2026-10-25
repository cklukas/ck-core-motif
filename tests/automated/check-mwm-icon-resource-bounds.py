#!/usr/bin/env python3
"""Check that WSM icon resource keys use bounded formatting."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text(
    encoding="utf-8"
)
start = source.index("BuildIconResource")
region = source[start:source.index("#endif /* WSM */", start)]
assert "snprintf(buffer, buffer_size" in region
assert "return length >= 0" in region
assert "sprintf(resourceBuf, iconXPosStr" not in region
assert "sprintf(resourceBuf, iconYPosStr" not in region
print("Mwm icon resource keys are bounded")
