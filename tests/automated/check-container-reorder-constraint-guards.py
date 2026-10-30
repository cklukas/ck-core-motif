#!/usr/bin/env python3
"""Check public Container reorder validates all constraint lookups."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("XmContainerReorder(\n\tWidget")
end = source.index("XmCreateContainer(\n", start)
region = source[start:end]
assert "if (c == NULL || c->node_ptr == NULL)" in region
assert "if (c != NULL && c->entry_parent == pcwid)" in region
assert "if (c != NULL && c->node_ptr != NULL && c->entry_parent == pcwid)" in region

print("PASS public Container reorder validates constraints")
