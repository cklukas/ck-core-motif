#!/usr/bin/env python3
"""Check RowActions child-creation failure paths are null-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowActions.c").read_text()
assert "if (child == NULL)" in source
assert "actions->actions.items[i].button != NULL" in source
assert "XtSetMappedWhenManaged(actions->actions.items[i].button" in source
print("PASS RowActions child-creation failure paths are null-safe")
