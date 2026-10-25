#!/usr/bin/env python3
"""Check that segmented children are managed only after model construction."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SegmentedControl.c").read_text()
start = source.index("Boolean XmSegmentedControlSetItems(")
end = source.index("\nCardinal XmSegmentedControlGetItemCount", start)
body = source[start:end]
create = body.index("copy[i].button = XmCreateToggleButton")
manage = body.index("for (i = 0; i < count; ++i)\n        XtManageChild")
assert manage > create
assert "later, still-uninitialized item slots" in body
print("PASS segmented children are managed after all item slots are initialized")
