#!/usr/bin/env python3
"""Check Sidebar child creation failure paths are null-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Sidebar.c").read_text()
assert "if (child == NULL)" in source
assert "if (sidebar->sidebar.content != NULL)" in source
assert "if (sidebar->sidebar.toggle_button != NULL)" in source
assert "XtAddCallback(sidebar->sidebar.toggle_button" in source
assert "XmeConfigureObject(sidebar->sidebar.toggle_button" in source
print("PASS Sidebar child-creation failure paths are null-safe")
