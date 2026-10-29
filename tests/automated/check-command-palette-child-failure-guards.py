#!/usr/bin/env python3
"""Check CommandPalette child-creation failure paths are null-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CommandPalette.c").read_text()
assert "if (child == NULL)" in source
assert "palette->palette.search == NULL" in source
assert "palette->palette.list == NULL" in source
assert "if (palette->palette.search != NULL)" in source
assert "if (palette->palette.list != NULL)" in source
assert "palette->palette.visible == NULL" in source
print("PASS CommandPalette child-creation failure paths are null-safe")
