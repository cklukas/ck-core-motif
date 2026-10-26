#!/usr/bin/env python3
"""Check TextField initialization guards essential allocations."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void \nValidates(XmTextFieldWidget tf)")
end = source.index("\n/*\n * Get the graphics context", start)
body = source[start:end]

assert "if (tf->text.highlight.list == NULL)" in body
assert 'XtError("Unable to allocate TextField highlight storage")' in body
assert "if (tf->text.onthespot == NULL)" in body
assert 'XtError("Unable to allocate TextField input-method storage")' in body

print("TextField initialization checks essential storage")
