#!/usr/bin/env python3
"""Check that XmGetXmScreen rejects a missing screen before dereferencing it."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c").read_text(
    encoding="utf-8"
)
start = text.index("XmGetXmScreen(\n        Screen *screen )")
body = text[start : text.index("}\n", start) + 2]
guard = "if (screen == NULL)\n\treturn(NULL);"
if guard not in body:
    raise SystemExit("XmGetXmScreen null-screen guard is missing")
if body.index(guard) > body.index("DisplayOfScreen(screen)"):
    raise SystemExit("XmGetXmScreen dereferences screen before its guard")

print("XmGetXmScreen null-screen guard present")
