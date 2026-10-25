#!/usr/bin/env python3
"""Check icon activation validates widget and context results."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "void IconActivateCallback" in text
assert "if (w == NULL)" in text
assert "&& pCD != NULL)" in text
print("icon activation validates widget and context results")
