#!/usr/bin/env python3
"""Check the Container destination prehook validates callback inputs first."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("ContainerDestPrehookProc(")
region = source[start:source.index("\n}", start)]
guard = region.index("if (wid == NULL || cs == NULL)")
assert guard < region.index("cs->selection"), "prehook reads callback data before its null guard"
print("PASS Container destination prehook validates widget and callback inputs")
