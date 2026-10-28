#!/usr/bin/env python3
"""Check DataField insertion callbacks reject incomplete selection state."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataFSel.c").read_text()
start = source.index("InsertSelection(\n        Widget")
body = source[start:source.index("/* ARGSUSED */", start + 1)]

assert "w == NULL" in body
assert "insert_select == NULL" in body
assert "type == NULL" in body
assert "length == NULL" in body
assert "format == NULL" in body
guard = body[body.index("if (w == NULL"):body.index("if (!value)")]
assert "XtFree((char *)value);" in guard

print("PASS DataField insertion callback validates selection inputs")
