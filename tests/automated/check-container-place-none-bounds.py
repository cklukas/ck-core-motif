#!/usr/bin/env python3
"""Check Container none-style placement widens content-bound arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("PlaceItemNone(\n    Widget      wid,")
start = source.index("{", start)
end = source.index("\n/************************************************************************", start)
region = source[start:end]

assert "int64_t content_right" in region
assert "int64_t content_bottom" in region
assert "(int64_t)cw->core.width -" in region
assert "(int64_t)cwid->core.x + (int64_t)cwid->core.width" in region
assert "cw->core.width - cw->container.margin_w" not in region
print("PASS Container none-style placement uses widened bounds")
