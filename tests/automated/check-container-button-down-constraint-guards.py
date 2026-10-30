#!/usr/bin/env python3
"""Check button-down transfer handling rejects missing item constraints."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("ContainerHandleBtn1Down(\n        Widget")
end = source.index("ContainerHandleBtn1Up(\n        Widget", start)
region = source[start:end]
assert "cwidc = GetContainerConstraint(cwid);" in region
assert "cwidc != (XmContainerConstraint) NULL" in region
assert region.index("cwidc != (XmContainerConstraint) NULL") < region.index("cwidc -> selection_state")

print("PASS Container button-down handling checks item constraints")
