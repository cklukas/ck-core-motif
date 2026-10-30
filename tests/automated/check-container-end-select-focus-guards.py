#!/usr/bin/env python3
"""Check Container end-select handles missing focus constraints."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("ContainerEndSelect(\n        Widget")
end = source.index("ContainerBeginToggle(\n        Widget", start)
region = source[start:end]
assert "GetContainerConstraint(current_focus);" in region
assert "if (c == NULL || c->selection_visual != XmSELECTED)" in region

print("PASS Container end-select handles missing focus constraints")
