#!/usr/bin/env python3
"""Check Text highlight capacity keeps meaningful allocation bounds."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
prototype = source.index("AddWidget(XmTextSource source,")
start = source.index("AddWidget(XmTextSource source,", prototype + 1)
end = source.index("RemoveWidget(XmTextSource source,", start)
body = source[start:end]

assert "highlight.maximum < 0" not in body
assert "UINT_MAX / sizeof(_XmHighlightRec)" in body

print("Text highlight capacity retains checked allocation bounds")
