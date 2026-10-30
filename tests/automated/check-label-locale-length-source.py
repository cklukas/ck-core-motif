#!/usr/bin/env python3
"""Ensure Label locale conversion measures its source, not its result."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Label.c").read_text(
    encoding="utf-8"
)
start = source.index("ConvertToEncoding(Widget w")
start = source.rfind("static XtPointer", 0, start)
body = source[start:source.index("\n}\n", start) + 2]
assert "*length = str ? strlen(str) : 0;" in body
assert "strlen(rval)" not in body
assert body.index("strlen(str)") < body.index("_XmTextToLocaleText(w")
print("PASS Label locale conversion measures source text safely")
