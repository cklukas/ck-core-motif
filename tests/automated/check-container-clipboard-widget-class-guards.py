#!/usr/bin/env python3
"""Check Container clipboard helpers validate their widget class."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
for name, end in (
    ("XmContainerCut(", "XmContainerCopy("),
    ("XmContainerCopy(", "XmContainerPaste("),
    ("XmContainerCopyLink(", "XmContainerPasteLink("),
):
    start = source.index(name)
    region = source[start:source.index(end, start)]
    assert "if (wid == NULL || !XmIsContainer(wid))" in region
    assert region.index("if (wid == NULL || !XmIsContainer(wid))") < region.index("_XmWidgetToAppContext(wid)")
print("PASS Container clipboard helpers validate widget class before toolkit access")
