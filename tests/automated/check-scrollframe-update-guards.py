#!/usr/bin/env python3
"""Check ScrollFrame navigator updates validate traits and list entries."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrollFrameT.c").read_text()
start = source.index("_XmSFUpdateNavigatorsValue(")
assert "if (sf == NULL)" in source[start:]
assert "sf_trait == NULL || sf_trait->getInfo == NULL" in source[start:]
assert "nav_list == NULL || count == 0" in source[start:]
assert "UINT_MAX / sizeof(*snapshot)" in source[start:]
assert "nav_trait != NULL && nav_trait->setValue != NULL" in source[start:]
assert "owner.destroyed || sf->core.being_destroyed" in source[start:]
assert "snapshot[i].destroyed" in source[start:]
assert "XtRemoveCallback" in source[start:]
print("PASS ScrollFrame navigator update guards are present")
