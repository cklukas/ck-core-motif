#!/usr/bin/env python3
"""Ensure TextField preedit startup has defined replacement endpoints."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text(
    encoding="utf-8"
)
start = source.index("PreeditStart(")
end = source.index("\n/*\n * This is the function set to XNPreeditDrawCallback", start)
region = source[start:end]

assert "cursorPos = nextPos = TextF_CursorPosition(tf);" in region
assert "_XmTextFieldReplaceText(tf," in region

print("TextField preedit startup initializes replacement endpoints")
