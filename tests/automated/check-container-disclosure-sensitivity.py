#!/usr/bin/env python3
"""Check disabled Container rows cannot change disclosure state."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nOutlineButtonAction(")
body = source[start : source.index("\n}\n\n\n/************************************************************************\n * MoveItemCallback", start) + 3]

guard = "if (cwid == NULL || !XtIsSensitive(cwid))"
if guard not in body:
    raise SystemExit("outline action lacks disabled-row guard")
if body.index(guard) > body.index("XmContainerWidget cw"):
    raise SystemExit("outline action accesses parent before sensitivity validation")
if "XtCallCallbackList" not in body:
    raise SystemExit("outline action body was not located correctly")

print("Container disclosure actions ignore disabled rows")
