#!/usr/bin/env python3
"""Ensure WSM PackString handles null input."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
start = text.index("PackString(MessageData data, String str)")
end = text.index("\n}\n", start)
function = text[start:end]
assert "if (str == NULL)" in function
assert "str = \"\";" in function
print("WSM PackString handles null input")
