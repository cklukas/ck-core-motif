#!/usr/bin/env python3
"""Check Container cursor navigation validates direction and node state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("CalcNextLocationCursor(\n")
region = source[start:source.index("\n}", start)]
assert region.index("if (wid == NULL || direction == NULL)") < region.index("_XmConvertActionParam")
assert "if (cw->container.last_node &&" in region
assert "if (cw->container.first_node == NULL" in region
print("PASS Container cursor navigation validates direction and node state")
