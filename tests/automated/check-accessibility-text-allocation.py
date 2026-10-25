#!/usr/bin/env python3
"""Check that accessibility text extraction checks its temporary buffer."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Accessible.c").read_text()
start = source.index("static char *text_impl(")
end = source.index("\n/* Semantic providers", start)
region = source[start:end]

assert "local = XtMalloc((Cardinal)capacity);" in region
assert "if (local == NULL)" in region
assert region.index("if (local == NULL)") < region.index("XmTextGetSubstring")
assert "XmTextFieldGetSubstring" in region

print("PASS accessibility text extraction checks temporary allocation")
