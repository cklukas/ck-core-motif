#!/usr/bin/env python3
"""Check RowColumn popup-menu input and shell allocation guards."""

from pathlib import Path


source = Path("lib/Xm/RowColumn.c").read_text()
start = source.index("create(\n        Widget p")
body = source[start:source.index("/****************", start)]

assert "if (p == NULL || name == NULL)" in body
assert "if (pop == NULL)" in body
assert body.index("if (pop == NULL)") < body.index(
    "pop->menu_shell.private_shell = True;")

print("RowColumn popup-menu creation validates inputs and shell allocation")
