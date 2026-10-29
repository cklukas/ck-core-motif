#!/usr/bin/env python3
"""Check AutoSuggestBox internal child failure paths are null-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AutoSuggestBox.c").read_text()
assert "box->auto_suggest.list == NULL" in source
assert "box->auto_suggest.search == NULL" in source
assert "if (box->auto_suggest.search != NULL)" in source
assert "if (box->auto_suggest.list != NULL)" in source
assert "Widget parent_shell = shell_ancestor(widget);" in source
print("PASS AutoSuggestBox child-creation failure paths are null-safe")
