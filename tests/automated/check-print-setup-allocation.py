#!/usr/bin/env python3
"""Check XmPrintSetup input, ancestor, and shell allocation guards."""

from pathlib import Path


source = Path("lib/Xm/PrintS.c").read_text()
start = source.index("XmPrintSetup(")
body = source[start:source.index("/************************************************************************\n *\n *  XmPrintToFile", start)]

assert "!video_widget || !print_screen || !print_shell_name" in body
assert "while(video_widget && !XtIsApplicationShell(video_widget))" in body
assert "if (!pappshell)" in body
assert "if (!print_shell)" in body
assert "XtDestroyWidget(pappshell)" in body

print("XmPrintSetup validates inputs and popup-shell allocation")
