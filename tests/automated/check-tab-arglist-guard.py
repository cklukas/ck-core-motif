#!/usr/bin/env python3
"""Check internal tab creation validates nonempty argument lists."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmTabList.c").read_text()

assert "tl == NULL || (argcount != 0 && arglist == NULL)" in source
assert "for (i = 0; i < argcount; i++)" in source

print("tab creation validates its ArgList input")
