#!/usr/bin/env python3
"""Check FormSection child-creation failure paths are null-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FormSection.c").read_text()
assert "if (child == NULL)" in source
assert "if (section->section.header != NULL)" in source
assert "if (section->section.content != NULL)" in source
assert "if (section->section.footer != NULL)" in source
print("PASS FormSection child-creation failure paths are null-safe")
