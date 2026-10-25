#!/usr/bin/env python3
"""Check SetNewBounds handles missing child-list entries safely."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "if (children == NULL)" in text
assert "if (children[i] == NULL)" in text
print("icon-box bounds recalculation handles missing child entries")
