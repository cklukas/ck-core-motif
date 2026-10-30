#!/usr/bin/env python3
"""Check XmContainerReorder validates widget and child-list inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("XmContainerReorder(")
region = source[start:]
assert "if (wid == NULL || !XmIsContainer(wid) ||" in region
assert "if (cwid_list[i] == NULL)" in region
assert "XtParent(cwid_list[0]) != wid" in region
assert "cwid_list[0]->core.constraints == NULL" in region
assert "XtParent(cwid_list[i]) != wid" in region
assert region.index("if (wid == NULL || !XmIsContainer(wid) ||") < region.index("_XmWidgetToAppContext(wid)")
print("PASS XmContainerReorder validates widget class and child-list inputs")
