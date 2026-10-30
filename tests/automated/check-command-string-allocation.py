#!/usr/bin/env python3
"""Ensure CommandCallback handles text-string allocation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/Command.c").read_text()
start = source.rindex("CommandCallback(")
body = source[start:]

assert "str = XmTextFieldGetString" in body
assert "if (str == NULL)" in body
assert body.index("if (str == NULL)") < body.index("XmStringGenerate")
print("CommandCallback handles text-string allocation failure")
