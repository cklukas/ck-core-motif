#!/usr/bin/env python3
"""Check public Container helpers reject non-Container widgets early."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")

start = source.index("XmContainerGetItemChildren(")
end = source.index("XmContainerRelayout(", start)
region = source[start:end]
assert "if (wid == NULL || !XmIsContainer(wid) || item_children == NULL)" in region
assert region.index("if (wid == NULL || !XmIsContainer(wid) || item_children == NULL)") < region.index("_XmWidgetToAppContext(wid)")

start = source.index("XmContainerRelayout(")
end = source.index("XmContainerReorder(", start)
region = source[start:end]
assert "if (wid == NULL || !XmIsContainer(wid))" in region
assert region.index("if (wid == NULL || !XmIsContainer(wid))") < region.index("_XmWidgetToAppContext(wid)")

print("PASS Container public helpers reject non-Container widgets before toolkit access")
