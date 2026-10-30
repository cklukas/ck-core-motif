#!/usr/bin/env python3
"""Check Container clipboard helpers validate their widget class."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
for name, end in (
    ("XmContainerCut(", "XmContainerCopy("),
    ("XmContainerCopy(", "XmContainerPaste("),
    ("XmContainerPaste(", "XmContainerCopyLink("),
    ("XmContainerCopyLink(", "XmContainerPasteLink("),
    ("XmContainerPasteLink(", "XmContainerGetItemChildren("),
):
    start = source.index(name)
    region = source[start:source.index(end, start)]
    assert "if (wid == NULL || !XmIsContainer(wid))" in region
    guard = region.index("if (wid == NULL || !XmIsContainer(wid))")
    if "_XmWidgetToAppContext(wid)" in region:
        assert guard < region.index("_XmWidgetToAppContext(wid)")
    else:
        assert guard < region.index("XmeClipboardSink(")
print("PASS Container clipboard helpers validate widget class before toolkit access")
