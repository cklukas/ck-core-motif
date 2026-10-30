#!/usr/bin/env python3
"""Check Container DnD placement validates callback and constraint inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("MoveItemCallback(\n        Widget")
region = source[start:]
assert "if (wid == NULL || client_data == NULL || call_data == NULL)" in region
assert "if (cwid == NULL || cs == NULL || cs->value == NULL)" in region
assert "if (c == NULL)" in region
assert region.index("if (c == NULL)") > region.index("c = GetContainerConstraint(cwid);")
assert region.index("if (c == NULL)") < region.index("dropspot->x")

print("PASS Container DnD placement validates callback inputs")
