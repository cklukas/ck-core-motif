#!/usr/bin/env python3
"""Ensure CommandCallback handles compound-string allocation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/Command.c").read_text()
body = source[source.rindex("CommandCallback("):]

assert "cb.value = XmStringGenerate" in body
assert "if (cb.value == NULL)" in body
assert body.index("if (cb.value == NULL)") < body.index("XmStringLength (cb.value)")
print("CommandCallback handles compound-string allocation failure")
