#!/usr/bin/env python3
"""Check the Container drop-finish callback validates its client payload."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("DropDoneCallback(")
region = source[start:source.index("\n}", start)]
guard = region.index("if (cw == NULL)")
assert guard < region.index("cw->container.drag_context"), "drop-finish callback writes through null client data"
print("PASS Container drop-finish callback validates client data")
