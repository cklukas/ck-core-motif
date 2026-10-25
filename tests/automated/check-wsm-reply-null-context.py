#!/usr/bin/env python3
"""Ensure WSM receive callbacks tolerate missing request context."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "send.c").read_text(
    encoding="utf-8"
)
assert "if (req_info == NULL)" in text
assert "Atom type;" in text
print("WSM receive callbacks guard missing request context")
