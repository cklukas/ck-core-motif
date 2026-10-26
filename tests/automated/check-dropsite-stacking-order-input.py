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
configure = source.index("XmDropSiteConfigureStackingOrder(")
configure_end = source.index("\nXmDropSiteVisuals \nXmDropSiteGetActiveVisuals", configure)
configure_region = source[configure:configure_end]
assert "if (widget == NULL)" in configure_region
assert "Cardinal i;" in configure_region
assert "else if (index < sib_index)" in configure_region
visuals = source.index("XmDropSiteVisuals \nXmDropSiteGetActiveVisuals(")
visuals_end = source.index("\nvoid \n", visuals + 1)
visuals_region = source[visuals:visuals_end]
assert "if (widget == NULL)" in visuals_region
assert "if (dsv == NULL)" in visuals_region
print("PASS DropSite stacking-order inputs and child indices are safe")
