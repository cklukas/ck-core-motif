#!/usr/bin/env python3
"""Check child enumeration validates constraint storage before dereference."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("XmContainerGetItemChildren(")
end = source.index("XmContainerRelayout(", start)
region = source[start:end]
assert "if (XtParent(item) != wid || item->core.constraints == NULL)" in region
assert region.index("item->core.constraints == NULL") < region.index("c = GetContainerConstraint(item)")
print("PASS Container child enumeration guards missing child constraints")
