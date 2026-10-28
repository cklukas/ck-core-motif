#!/usr/bin/env python3
"""Ensure DropDown popup event dispatch tolerates foreign windows."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropDown.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nShellButtonEvent(")
body = source[start:source.index("\n}\n", start)]
assert "if (cbw == NULL || event == NULL || dispatch == NULL)" in body
assert "if (event_widget == NULL)" in body
assert "while (event_shell != NULL && !XtIsShell(event_shell))" in body
assert "if (event_shell == NULL)" in body
print("DropDown grabbed-event dispatch guards widget lookup")
