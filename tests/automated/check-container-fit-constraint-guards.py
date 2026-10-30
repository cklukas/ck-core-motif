#!/usr/bin/env python3
"""Check Container fit testing rejects children without constraints."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("TestFitItem(\n    Widget")
end = source.index("PlaceItem(", start)
region = source[start:end]
lookup = "XmContainerConstraint cwidc = GetContainerConstraint(cwid);"
guard = "if (cwidc == NULL)"
assert lookup in region
assert guard in region
assert region.index(guard) > region.index(lookup)
assert region.index(guard) < region.index("if (CtrLayoutIsOUTLINE_DETAIL(cw))")

print("PASS Container fit testing rejects children without constraints")
