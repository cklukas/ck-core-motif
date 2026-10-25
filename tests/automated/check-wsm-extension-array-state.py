#!/usr/bin/env python3
"""Ensure WSM extension packers validate their pointer arrays."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
assert "request->extensions.extension_suggestions == NULL" in text
assert "reply->extensions.extensions == NULL" in text
print("WSM extension packers validate pointer arrays")
