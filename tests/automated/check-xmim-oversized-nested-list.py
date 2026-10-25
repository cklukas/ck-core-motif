#!/usr/bin/env python3
"""Check that oversized XmIm nested lists do not abort via assert."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()

assert "if (count > 10)" in source
body = source[source.index("VaCopy(VaArgList list)"):source.index("static void\nVaSetArg")]
assert "return NULL;" in body
assert "assert(count <= 10)" not in source

print("XmIm oversized nested-list guard: ok")
