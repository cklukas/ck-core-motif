#!/usr/bin/env python3
"""Reject patch markers that would make CommandPalette.c invalid C."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CommandPalette.c").read_text(
    encoding="utf-8"
)
assert "\n+static XmString make_display_string" not in source
assert "static XmString make_display_string" in source
print("PASS CommandPalette source contains no patch markers")
