#!/usr/bin/env python3
"""Check XmContainerReorder validates widget and child-list inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("XmContainerReorder(")
region = source[start:]
assert "if (wid == NULL || cwid_count <= 1 || cwid_list == NULL)" in region
assert "if (cwid_list[i] == NULL)" in region
assert region.index("if (wid == NULL || cwid_count <= 1 || cwid_list == NULL)") < region.index("_XmWidgetToAppContext(wid)")
print("PASS XmContainerReorder validates widget and child-list inputs")
