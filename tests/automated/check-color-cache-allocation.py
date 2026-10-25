#!/usr/bin/env python3
"""Check dynamic default-color cache growth and fallback are guarded."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmResource.c"
text = source.read_text(encoding="utf-8")

assert "XmColorData *new_set" in text
assert "if (new_set == NULL)" in text
assert "if (defaultColors == NULL)" in text
assert "newValue = newBackground;" in text
print("default-color cache allocation and fallback are guarded")
