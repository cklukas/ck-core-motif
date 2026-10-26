#!/usr/bin/env python3
"""Check DropSite stacking-order traversal validates outputs and counts."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropSMgr.c").read_text(encoding="utf-8")
start = source.index("XmDropSiteQueryStackingOrder(")
end = source.index("\nvoid \nXmDropSiteConfigureStackingOrder", start)
region = source[start:end]

assert "parent_rtn == NULL || children_rtn == NULL" in region
assert "*parent_rtn = NULL;" in region
assert "Cardinal i, j, child_count;" in region
assert "child_count = GetDSNumChildren(info);" in region
assert "for (j = 0, i = child_count; i != 0; )" in region
assert "--i;" in region
print("PASS DropSite stacking-order inputs and child indices are safe")
