#!/usr/bin/env python3
"""Check WSM cleanup consumes paired properties without count multiplication."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/pack.c"
text = source.read_text(encoding="utf-8")

assert "j < num * 2" not in text
assert text.count("for (j = 0; j < num; j++) {") >= 2
assert text.count("(void) UnpackProperty(&data);") >= 4
print("WSM paired-property cleanup avoids count multiplication")
