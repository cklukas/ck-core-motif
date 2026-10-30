#!/usr/bin/env python3
"""Check XmContainerReorder rejects children outside its Container."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("XmContainerReorder(")
region = source[start:]
assert "if (XtParent(cwid_list[0]) != wid ||" in region
assert "cwid_list[0]->core.constraints == NULL" in region
assert "if (XtParent(cwid_list[i]) != wid ||" in region
assert "cwid_list[i]->core.constraints == NULL" in region
assert region.index("XtParent(cwid_list[0]) != wid") < region.index("c = GetContainerConstraint(cwid_list[0])")
print("PASS XmContainerReorder validates child parents before constraint access")
