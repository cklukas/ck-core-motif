#!/usr/bin/env python3
"""Check Container clipboard commands validate their widgets."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
for signature in ("XmContainerCut(", "XmContainerCopy(", "XmContainerCopyLink("):
    start = source.index(signature)
    region = source[start:]
    assert "if (wid == NULL)" in region
    assert region.index("if (wid == NULL)") < region.index("_XmWidgetToAppContext(wid)")
print("PASS Container clipboard commands validate null widgets")
