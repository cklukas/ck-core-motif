#!/usr/bin/env python3
"""Check the Container outline callback validates widget and event inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("OutlineButtonCallback(")
region = source[start:source.index("\n}", start)]
guard = region.index("if (cwid == NULL || pbcbs == NULL)")
assert guard < region.index("GetContainerConstraint"), "outline callback reads widget state before its null guard"
assert region.index("if (c == NULL)") < region.index("c->outline_state"), "outline callback dereferences a missing constraint"
print("PASS Container outline callback validates widget, payload, and constraint")
