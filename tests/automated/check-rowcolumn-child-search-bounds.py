#!/usr/bin/env python3
"""Check RowColumn managed-child search uses full-width count variables."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowColumn.c").read_text(
    encoding="utf-8"
)
start = source.index("FindFirstManagedChild(\n        CompositeWidget m,")
start = source.index("{", start)
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "register Cardinal i = 0;" in region
assert "register Cardinal n;" in region
assert "register int i" not in region
assert "register int n" not in region
assert "n = m->composite.num_children;" in region
print("RowColumn managed-child search uses full-width Cardinal counts")
