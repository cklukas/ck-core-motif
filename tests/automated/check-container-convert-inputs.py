#!/usr/bin/env python3
"""Check the Container selection conversion callback validates its inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("ContainerConvertProc(")
region = source[start:source.index("\n}", start)]
guard = region.index("if (wid == NULL || cs == NULL)")
assert guard < region.index("XInternAtoms"), "conversion callback uses inputs before its null guard"
assert guard < region.index("cs->target"), "conversion callback reads callback data before its null guard"
print("PASS Container conversion callback validates widget and callback inputs")
