#!/usr/bin/env python3
"""Check Container grid placement widens content-bound arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("PlaceItemGridCells(\n    Widget      wid,")
start = source.index("{", start)
end = source.index("\n/************************************************************************", start)
region = source[start:end]

assert "int64_t content_right" in region
assert "int64_t content_bottom" in region
assert "(int64_t)place_point.x + (int64_t)cwid->core.width" in region
assert "(int64_t)place_point.y + (int64_t)cwid->core.height" in region
assert "cw->core.width - cw->container.margin_w" not in region
print("PASS Container grid placement uses widened bounds")
