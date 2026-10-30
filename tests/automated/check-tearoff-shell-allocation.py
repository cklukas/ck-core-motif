#!/usr/bin/env python3
"""Check tear-off shell creation is validated before shell use."""

from pathlib import Path


source = Path("lib/Xm/TearOff.c").read_text()
start = source.index("_XmTearOffInitiate(")
body = source[start:source.index("/*ARGSUSED*/", start)]

assert "to_shell = (ShellWidget)XtCreatePopupShell" in body
assert "if (to_shell == NULL)" in body
assert body.index("if (to_shell == NULL)") < body.index(
    "XmeSetWMShellTitle")
assert body.index("if (to_shell == NULL)") < body.index(
    "XInternAtoms")

print("Tear-off shell allocation is validated before use")
