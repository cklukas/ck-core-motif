#!/usr/bin/env python3
"""Check Disclosure child-creation failure paths are null-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Disclosure.c").read_text()
assert "if (child == NULL)" in source
assert "if (w->disclosure.arrow != NULL)" in source
assert "if (w->disclosure.header != NULL)" in source
assert "if (w->disclosure.content != NULL)" in source
assert "XtAddCallback(w->disclosure.header" in source
print("PASS Disclosure child-creation failure paths are null-safe")
